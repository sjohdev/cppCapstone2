#include <iostream>
#include <format>
#include <string>
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
}

namespace FileIO {
    
}

namespace Utility {
    
    void listDataFiles(int dataChoice) {
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
            fileExt = ".wav";
        } else if (dataChoice == 2) {
            // Image processing: look for .jpeg files
            fileExt = ".jpg";
        } else {
            // Invalid input detected: Abort
            return;
        }
        
        if (std::filesystem::exists(sourcePath) && std::filesystem::is_directory(sourcePath)) {
            std::cout << "Directory '" << pathName << "': " << std::endl;
            // List all files with correct file extension: 
            for (const auto& dirEntry : std::filesystem::directory_iterator(sourcePath)) {
                fileName = dirEntry.path().filename().string();
                // Check if currect file matches correct filepattern:
                isDataFile = (fileName.find(fileExt) != std::string::npos);
                if (isDataFile) {
                    fileCount++;
                    // Print filename
                    std::cout << "\n" << fileCount << ". " << fileName;
                }
            }
            // Print number of files with correct data type:
            std::cout << std::format("\nFound #{} valid data files of type ({}).\n", fileCount, fileExt);
        } else {
            std::cerr << "Error: Directory not found." << std::endl;
        }
    }
}

int main() {
    // Main-menu loop: 
    int dataChoice;
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
            Utility::listDataFiles(dataChoice);
            
            // Prompt user to process found files or abort
            
            // Process files if user chose to do so (otherwise skip)
            
        }
    } while(dataChoice > 0);
    return 0;
}

