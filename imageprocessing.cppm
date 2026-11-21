module;

#include <string>
#include <vector>

export module imageprocessing;

namespace ImageProc {
    export void processFiles(const std::vector<std::string>& filePathList);
}

