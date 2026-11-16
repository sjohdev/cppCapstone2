
#include "audio_test.h"
#include <iostream>
#include <iomanip>
#include <algorithm>

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
    std::cout << "Audio format: "
    << (info.audio_format == 1 ? "PCM" : "IEEE Float")
    << '\n';

    std::cout << "Data size: " << info.data_size << " bytes\n";

    const size_t num_frames =
    wav.samples.size() / info.channels;

    std::cout << "Audio frames: " << num_frames << '\n';

    const double duration =
    static_cast<double>(num_frames) / info.sample_rate;

    std::cout << "Duration: "
    << std::fixed << std::setprecision(3)
    << duration << " seconds\n";

    // Display first 20 individual samples
    const size_t num_samples =
    std::min<size_t>(20, wav.samples.size());

    std::cout << "\nFirst " << num_samples
    << " audio samples:\n";

    for (size_t i = 0; i < num_samples; ++i)
    {
    std::cout << std::setw(4) << i
    << ": "
    << std::fixed << std::setprecision(6)
    << wav.samples[i] << '\n';
    }

    std::cout << "================================\n\n";
}
