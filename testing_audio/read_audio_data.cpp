
#include "audio_test.h"
#include <fstream>
#include <vector>
#include <cstdint>
#include <cstring>
#include <cmath>
#include <stdexcept>
#include <limits>

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

