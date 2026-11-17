module;

#include <fstream>
#include <stdexcept>
#include <string>
#include <cstdint>
#include <limits>

#include <vector>
#include <cstring>
#include <cmath>

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <exception>


module audioprocessing;

// Functions for reading wav files and retrieveng the meta-data and audio samples
// NOTE: This module was developed with the help of AI (chatGPT-6 Luna) 

namespace AudioProc {

    struct WavInfo
    {
        uint32_t file_size = 0;
        
        uint16_t audio_format = 0;
        uint16_t channels = 0;
        uint32_t sample_rate = 0;
        uint32_t byte_rate = 0;
        uint16_t block_align = 0;
        uint16_t bits_per_sample = 0;
        
        uint32_t data_size = 0;
        uint64_t data_offset = 0;
    };

    struct WavFile 
    {
        std::string filename;
        WavInfo info;
        
        // Normalized audio samples between -1.0 to 1.0: 
        std::vector<float> samples;
    };

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
    
    void read_audio_data(WavFile& wav)
    {
        std::ifstream file(wav.filename, std::ios::binary);

        if (!file)
            throw std::runtime_error("Cannot open WAV file");

        const auto& info = wav.info;

        if (info.data_size > static_cast<uint64_t>(std::numeric_limits<std::streamsize>::max()))
            throw std::runtime_error("Audio data too large");

        file.seekg(static_cast<std::streamoff>(info.data_offset));

        if (!file)
            throw std::runtime_error("Cannot seek to audio data");

        std::vector<unsigned char> raw_data(info.data_size);

        if (!file.read(reinterpret_cast<char*>(raw_data.data()),static_cast<std::streamsize>(raw_data.size())))
            throw std::runtime_error("Error reading audio data");

        const uint16_t bits = info.bits_per_sample;
        const uint16_t format = info.audio_format;

        wav.samples.clear();

        const size_t bytes_per_sample = bits / 8;
        const size_t num_samples =
        raw_data.size() / bytes_per_sample;

        wav.samples.reserve(num_samples);

        for (size_t i = 0; i < raw_data.size(); i += bytes_per_sample)
        {
            float sample = 0.0f;

            if (format == 1) // Integer PCM
            {
                if (bits == 8)
                {
                // 8-bit PCM is unsigned
                sample =
                (static_cast<int>(raw_data[i]) - 128)
                / 128.0f;
                }
                else if (bits == 16)
                {
                uint16_t value =
                uint16_t(raw_data[i]) |
                (uint16_t(raw_data[i + 1]) << 8);

                int16_t signed_value;
                std::memcpy(&signed_value, &value, 2);

                sample = signed_value / 32768.0f;
                }
                else if (bits == 24)
                {
                int32_t value =
                int32_t(raw_data[i]) |
                (int32_t(raw_data[i + 1]) << 8) |
                (int32_t(raw_data[i + 2]) << 16);

                // Sign extension from 24 to 32 bits
                if (value & 0x800000)
                value |= static_cast<int32_t>(0xFF000000u);

                sample = value / 8388608.0f;
                }
                else if (bits == 32)
                {
                    uint32_t value =
                    uint32_t(raw_data[i]) |
                    (uint32_t(raw_data[i + 1]) << 8) |
                    (uint32_t(raw_data[i + 2]) << 16) |
                    (uint32_t(raw_data[i + 3]) << 24);

                    int32_t signed_value;
                    std::memcpy(&signed_value, &value, 4);

                    sample = static_cast<float>(static_cast<double>(signed_value) / 2147483648.0);
                }
            }
            else if (format == 3 && bits == 32)
            {
                // IEEE floating-point WAV
                uint32_t value =
                uint32_t(raw_data[i]) |
                (uint32_t(raw_data[i + 1]) << 8) |
                (uint32_t(raw_data[i + 2]) << 16) |
                (uint32_t(raw_data[i + 3]) << 24);

                std::memcpy(&sample, &value, sizeof(sample));

                if (!std::isfinite(sample))
                    throw std::runtime_error("Invalid floating-point sample");
            }

            wav.samples.push_back(sample);
        }
    }
    
    void display_wav_info(const WavFile& wav)
    {
        const auto& info = wav.info;

        std::cout << "\n================================\n";
        std::cout << " WAV FILE INFORMATION\n";
        std::cout << "================================\n";

        std::cout << "Filename: " << wav.filename << '\n';
        std::cout << "File size: " << info.file_size << " bytes\n";
        std::cout << "Channels: " << info.channels << '\n';
        std::cout << "Sample rate: " << info.sample_rate << " Hz\n";
        std::cout << "Bits per sample:" << info.bits_per_sample << '\n';
        std::cout << "Audio format: " << (info.audio_format == 1 ? "PCM" : "IEEE Float") << std::endl;
        std::cout << "Data size: " << info.data_size << " bytes\n";

        const size_t num_frames = wav.samples.size() / info.channels;
        std::cout << "Audio frames: " << num_frames << std::endl;

        const double duration = static_cast<double>(num_frames) / info.sample_rate;
        std::cout << "Duration: " << std::fixed << std::setprecision(3) << duration << " seconds\n";

        // Display first 20 individual samples
        const size_t num_samples = std::min<size_t>(20, wav.samples.size());
        std::cout << "\nFirst " << num_samples << " audio samples:\n";

        for (size_t i = 0; i < num_samples; ++i)
        {
            std::cout << std::setw(4) << i << ": " << std::fixed << std::setprecision(6) 
            << wav.samples[i] << std::endl;
        }
        std::cout << "================================\n" << std::endl;
    }
    
    void processFiles(std::vector<std::string>& filePathList) {
        std::cout << "Processing Audio-files..." << std::endl;
        // for each file in filepathlist: 
        size_t fileCount = 0;
        for (const std::string& filename : filePathList) {
            WavFile wav;
            try {
                // Parse header
                parse_wav_file(filename, wav);
                
                // retrieve data samples
                read_audio_data(wav);
                
                // Display summary
                std::cout << "File #" << ++fileCount << ": " << std::endl;
                display_wav_info(wav);
            }
            catch(const std::exception& e) {
                std::cerr << "Error: " << e.what() << '\n';
                return;
            }
        }
    }
}
