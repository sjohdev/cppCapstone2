#include "audio_test.h"
#include <fstream>
#include <stdexcept>
#include <string>
#include <cstdint>
#include <limits>

uint16_t read_u16(std::istream& file)
{
    unsigned char b[2];

    if (!file.read(reinterpret_cast<char*>(b), 2))
        throw std::runtime_error("Unexpected end of file");

    return uint16_t(b[0]) |
           (uint16_t(b[1]) << 8);
}

uint32_t read_u32(std::istream& file)
{
    unsigned char b[4];

    if (!file.read(reinterpret_cast<char*>(b), 4))
        throw std::runtime_error("Unexpected end of file");

    return uint32_t(b[0]) |
           (uint32_t(b[1]) << 8) |
           (uint32_t(b[2]) << 16) |
           (uint32_t(b[3]) << 24);
}

void parse_wav_file(const std::string& filename, WavFile& wav)
{
    std::ifstream file(filename, std::ios::binary);

    if (!file)
        throw std::runtime_error("Cannot open WAV file");

    char riff[4], wave[4];

    if (!file.read(riff, 4))
        throw std::runtime_error("Invalid RIFF header");

    uint32_t riff_size = read_u32(file);

    if (!file.read(wave, 4))
        throw std::runtime_error("Invalid WAVE header");

    if (std::string(riff, 4) != "RIFF" ||
        std::string(wave, 4) != "WAVE")
        throw std::runtime_error("Not a RIFF/WAVE file");

    // Determine physical file size
    file.seekg(0, std::ios::end);
    auto physical_size = file.tellg();

    if (physical_size < 0)
        throw std::runtime_error("Cannot determine file size");

    const uint64_t file_size =
        static_cast<uint64_t>(physical_size);

    const uint64_t riff_end = uint64_t(riff_size) + 8;

    if (riff_end > file_size || riff_end < 12)
        throw std::runtime_error("Invalid RIFF size");

    wav.filename = filename;
    wav.info.file_size = riff_size + 8;

    bool found_fmt = false;
    bool found_data = false;

    // Start reading chunks after RIFF header
    file.seekg(12);

    while (static_cast<uint64_t>(file.tellg()) < riff_end)
    {
        char chunk_id[4];

        if (!file.read(chunk_id, 4))
            throw std::runtime_error("Invalid chunk header");

        uint32_t chunk_size = read_u32(file);

        const uint64_t chunk_start =
            static_cast<uint64_t>(file.tellg());

        const uint64_t padded_size =
            uint64_t(chunk_size) + (chunk_size & 1u);

        if (chunk_start > riff_end ||
            padded_size > riff_end - chunk_start)
            throw std::runtime_error("Chunk exceeds RIFF boundary");

        std::string id(chunk_id, 4);

        if (id == "fmt ")
        {
            if (found_fmt)
                throw std::runtime_error("Duplicate fmt chunk");

            if (chunk_size < 16)
                throw std::runtime_error("Invalid fmt chunk");

            wav.info.audio_format = read_u16(file);
            wav.info.channels = read_u16(file);
            wav.info.sample_rate = read_u32(file);
            wav.info.byte_rate = read_u32(file);
            wav.info.block_align = read_u16(file);
            wav.info.bits_per_sample = read_u16(file);

            found_fmt = true;
        }
        else if (id == "data")
        {
            if (found_data)
                throw std::runtime_error("Multiple data chunks unsupported");

            wav.info.data_size = chunk_size;
            wav.info.data_offset = chunk_start;

            found_data = true;
        }

        // Move to the next chunk, accounting for padding
        file.seekg(
            static_cast<std::streamoff>(chunk_start + padded_size)
        );

        if (!file)
            throw std::runtime_error("Error seeking WAV chunk");
    }

    if (!found_fmt)
        throw std::runtime_error("Missing fmt chunk");

    if (!found_data)
        throw std::runtime_error("Missing data chunk");

    if (wav.info.channels == 0 ||
        wav.info.sample_rate == 0 ||
        wav.info.block_align == 0)
        throw std::runtime_error("Invalid audio parameters");

    if (wav.info.audio_format != 1 &&
        wav.info.audio_format != 3)
        throw std::runtime_error("Unsupported WAV audio format");

    // Validate supported sample representations
    const auto format = wav.info.audio_format;
    const auto bits = wav.info.bits_per_sample;

    if ((format == 1 &&
         bits != 8 && bits != 16 && bits != 24 && bits != 32) ||
        (format == 3 && bits != 32))
        throw std::runtime_error("Unsupported sample format");

    const uint32_t expected_align =
        wav.info.channels * (bits / 8);

    if (wav.info.block_align != expected_align)
        throw std::runtime_error("Invalid block alignment");

    if (wav.info.byte_rate !=
        wav.info.sample_rate * wav.info.block_align)
        throw std::runtime_error("Invalid byte rate");

    if (wav.info.data_size % wav.info.block_align != 0)
        throw std::runtime_error("Incomplete audio frame");
}

