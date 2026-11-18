
#include <opencv2/opencv.hpp>
//#include <torch/torch.h>
#include <iostream>
#include <string>
#include <exception>

//#include "image_test.h"

int main() {
    std::string filename = "/home/badblob/coding/deeplearning/imsearch/data/spider2.jpg";
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
        
        // convert image to tensor
        
        // 1. BGR to RGB
        /*cv::cvtColor(img, img, cv::COLOR_BGR2RGB);
        
        // 2. uint8 [0 255] to float [0 1]
        img.convertTo(img, CV_32FC3, 1.0 / 255.0);
        
        // 3. create tensor
        auto tensor = torch::from_blob(
            img.data, 
            {img.rows, img.cols, 3}, 
            torch::TensorOptions().dtype(torch::kFloat32)
            );
            
        // 4. [Rows x Cols x Channels] to [Channels x Rows x Cols]
        tensor = tensor.permute({2, 0, 1}).contiguous().clone();
        
        // 5. Add batch dimension
        tensor = tensor.unsqueeze(0);
        
        std::cout << "Tensor shape: " << tensor.sizes() << std::endl;
        
        // classify image with Pytorch model
        
        
        // Display class label*/
        
        
    }
    catch(const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;    
}
