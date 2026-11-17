module;

#include <string>
#include <vector>

export module audioprocessing;

namespace AudioProc {
    export void processFiles(std::vector<std::string>& filePathList);
    
}
