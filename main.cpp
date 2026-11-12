#include <iostream>
#include <format>
#include <string>
#include <vector>
#include <filesystem>

namespace UserInput {

    void getUserInput(auto& dataVar, auto minVal, auto maxVal) {
        // Check for inputstream failure and invalid user input at the same time:
        // IMPORTANT: the parenthesis around the !() statement matters
        while (!(std::cin >> dataVar) || dataVar < minVal || dataVar > maxVal) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Error: Invalid selection. Try again: ";
        }
    }
    
    void getUserInput(auto& dataVar) {
        // Check for inputstream failure and invalid user input at the same time:
        // IMPORTANT: the parenthesis around the !() statement matters
        while (!(std::cin >> dataVar)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Error: Invalid input. Try again: ";
        }
    }
    
    void getFilePath(std::string& filePathStr) {
        std::cout << "Enter full filepath to data folder: ";
        getUserInput(filePathStr);
    }
    
    int selectDataType() {
        int userSelection;
        std::cout << "1. Audio processing (.wav)" << std::endl;
        std::cout << "2. Image analysis (.jpeg)" << std::endl;
        std::cout << "0. Exit" << std::endl;
        std::cout << "Select: ";
        getUserInput(userSelection, 0, 2);
        return userSelection;
    }
    
    bool proceedYesNo() {
        std::cout << "Process files (y/Y) or return to main menu (n/N)? ";
        char answer;
        getUserInput(answer);
        return (answer == 'y' || answer == 'Y');
    }
}

namespace FileIO {
    
}

namespace Utility {

    size_t getDataFiles(std::vector<std::string>& filePathList, int dataChoice) {
        // Get file path from user: 
        std::string pathName;
        UserInput::getFilePath(pathName);
        
        std::string fileName;
        std::filesystem::path sourcePath = pathName;
        bool isDataFile = false;
        size_t fileCount = 0;
        
        std::string fileExt;
        if (dataChoice == 1) {
            // Audio processing: look for .wav files
            fileExt = ".wav"; // change to regex matching for improved flexibility later on
        } else if (dataChoice == 2) {
            // Image processing: look for .jpeg files
            fileExt = ".jpg"; // change to regex matching for improved flexibility later on
        } else {
            // Invalid input detected: Abort
            return 0;
        }
        
        if (std::filesystem::exists(sourcePath) && std::filesystem::is_directory(sourcePath)) {
            std::cout << "Directory '" << pathName << "': " << std::endl;
            // List all files with correct file extension: 
            for (const auto& dirEntry : std::filesystem::directory_iterator(sourcePath)) {
                fileName = dirEntry.path().filename().string();
                //fileName = dirEntry.path().string();
                // Check if currect file matches correct filepattern:
                isDataFile = (fileName.find(fileExt) != std::string::npos);
                if (isDataFile) {
                    fileCount++;
                    // Print filename
                    std::cout << "\n" << fileCount << ". " << fileName;
                    // Add filepath to list of valid file locations
                    filePathList.push_back(dirEntry.path().string());
                }
            }
            // Print number of files with correct data type:
            std::cout << std::format("\nFound #{} valid data files of type ({}).\n", fileCount, fileExt);
        } else {
            std::cerr << "Error: Directory not found." << std::endl;
        }
        return fileCount;
    }
}

namespace ImageProc {

    void processFiles(std::vector<std::string>& filePathList) {
        std::cout << "Processing Image-files..." << std::endl;
        // for each file in filepathlist: 
            
            // open file with FileIO namespace
            
            // process file 
    }
}

namespace AudioProc {

    void processFiles(std::vector<std::string>& filePathList) {
        std::cout << "Processing Audio-files..." << std::endl;
        // for each file in filepathlist: 
            
            // open file with FileIO namespace
            
            // process file 
    }
}

int main() {
    // Main-menu loop: 
    int dataChoice;
    bool proceed = false;
    do {
        // Show main title:
        std::cout << std::string(50, '=') << std::endl;
        std::cout << std::string(12, ' ') << "Data Processing Tool:" << std::endl;
        std::cout << std::string(50, '=') << std::endl;
    
        // Select data type to process: 
        dataChoice = UserInput::selectDataType();
        std::cout << std::format("\nYou chose option #{}\n", dataChoice);
        
        if (dataChoice > 0) {
            // List all files of selected data type in chosen data folder:
            std::vector<std::string> filePathList;
            size_t numFiles = Utility::getDataFiles(filePathList, dataChoice);
            
            if (numFiles > 0) {
                proceed = UserInput::proceedYesNo();
            } else {
                proceed = false;
            }
            
            // Process files if user chose to proceed (otherwise skip)
            if (proceed && dataChoice == 1) {
                // Process audiofiles:
                AudioProc::processFiles(filePathList); 
            } else if (proceed && dataChoice == 2) {
                // Process imagefiles:
                ImageProc::processFiles(filePathList);
            } else {
                std::cout << "Back to main menu:" << std::endl;
            }
            filePathList.clear();
        }
    } while(dataChoice > 0);
    return 0;
}

