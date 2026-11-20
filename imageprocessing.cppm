module;

#include <string>
#include <vector>

export module imageprocessing;

namespace ImageProc {
    export void processFiles(std::vector<std::string>& filePathList);
}

