
#include <cstdint>
#include <string>
#include <vector>

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

uint16_t read_u16(std::istream& file);
uint32_t read_u32(std::istream& file);
void parse_wav_file(const std::string& filename, WavFile& wav);
void display_wav_info(const WavFile& wav);
void read_audio_data(WavFile& wav);
