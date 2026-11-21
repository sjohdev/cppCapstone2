#include <iostream>
#include <string>
#include <format>
#include <fstream>

std::string getLastWord(std::string inputStr, std::string delimiter = " ") {
    size_t pos;
    size_t offset = 0;
    size_t strLength = inputStr.size();
    while (offset < strLength && (pos = inputStr.find(delimiter, offset)) != std::string::npos) {
        offset = pos + 1;
    }
    return inputStr.substr(offset);
}

void getUserInput(auto& dataVar, auto minVal, auto maxVal) {
    // Check for inputstream failure and invalid user input at the same time:
    // IMPORTANT: the parenthesis around the !() statement matters
    while (!(std::cin >> dataVar) || dataVar < minVal || dataVar > maxVal) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Error: Invalid selection. Try again: ";
    }
}

void getimlabel(int labelIndex) {
    // Call bash find method and store result in logfile: 
    const std::string searchCommand = std::format(
        "grep -w {}: ../deeplearning/imagenet1000_clsidx_to_labels.txt > labels_log.txt", 
        labelIndex
        );
        
    // Make sure system process is available
    bool procReady = not(std::system(nullptr) == 0);
    if (procReady) {
        // Run search command using the Unix terminal
        std::system(searchCommand.c_str());
    }
    
    // format searchresult: 
    std::string resultStr;
    std::string lastWord;
    std::ifstream readStream("labels_log.txt");
    std::getline(readStream, resultStr);
    lastWord = getLastWord(resultStr);
    //lastWord = getLastWord(resultStr, " ");

    // Confirm user input, search command, and search result: 
    std::cout << "Search command: " << searchCommand << std::endl;
    std::cout << "Search results: " << std::endl;
    std::cout << lastWord << std::endl;
    //std::cout << std::ifstream("labels_log.txt").rdbuf(); 
}

int main() {
    int labelIndex = 0;
    // User prompt: image label to check 
    std::cout << "Enter ImageNet class index to check (0-999): ";
    getUserInput(labelIndex, 0, 999);

    // Extract label descriptions using grep: 
    getimlabel(labelIndex);
    return 0;
}
