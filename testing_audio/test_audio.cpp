
#include <iostream>
#include <string>
#include <exception>

#include "audio_test.h"

int main() {
    WavFile wav;
    std::string fileaname = "mytest.wav";
    try {
        parse_wav_file(fileaname, wav);
        
        read_audio_data(wav);
        
        display_wav_info(wav);
    }
    catch(const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;    
}
