
#include <torch/torch.h>
#include "CImg.h"
#include <iostream>
#include <string>
#include <exception>

//#include "image_test.h"

int main() {
    const std::string filename = "/home/badblob/coding/deeplearning/imsearch/data/spider2.jpg";
    
    torch::Tensor tensor = torch::rand({2, 3});
    std::cout << tensor << std::endl;
    
    try {
    
        // initiate PyTorch image-classifier
        
        // Read image
        // Load an image, transform it to a color image (if necessary)
        cimg_library::CImg<unsigned char> image(filename.c_str());
        std::cout << "image dimensions: " 
        << image.width() << " x " 
        << image.height() << " x " 
        << image.spectrum() 
        << std::endl;
        
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
        
        
        // Display class label
        */
        
    }
    catch(const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;    
}
