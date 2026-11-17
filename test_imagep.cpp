
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <exception>

//#include "image_test.h"

int main() {
    std::string filename = "../deeplearning/imsearch/data/spider2.jpg";
    try {
    
        // initiate PyTorch image-classifier
        
        // Read image
        cv::Mat img = cv::imread(filename);
        if (img.empty()) {
            std::cout << "Failed to load image. Exiting" << std::endl;
            return 1;
        }
        else {
            std::cout << "Image loaded successfully!" << std::endl;
            return 0;
        }
        
        // classify image with Pytorch model
        
        
        // Display class label
        
        
    }
    catch(const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;    
}
