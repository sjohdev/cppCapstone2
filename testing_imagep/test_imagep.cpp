
#include <torch/torch.h>
#include "CImg.h"
#include <iostream>
#include <string>
#include <exception>

// NOTE: This function was developed with the help of AI (chatGPT-6 Luna) 

int main() {
    const std::string filename = "/home/badblob/coding/deeplearning/imsearch/data/spider2.jpg";
    
    torch::Tensor tensor = torch::rand({2, 3});
    std::cout << tensor << std::endl;
    
    try {
    
        // load PyTorch image-classifier model
        
        // Read image
        // Load an image, transform it to a color image (if necessary)
        cimg_library::CImg<unsigned char> image(filename.c_str());
        std::cout << "image dimensions: " 
        << image.width() << " x " 
        << image.height() << " x " 
        << image.spectrum() 
        << std::endl;
        
        // Convert image to tensor
        const int64_t width = image.width();
        const int64_t height = image.height();
        const int64_t channels = image.spectrum();
        
        
        // 1. create tensor
        auto tensor = torch::from_blob(
            const_cast<unsigned char*>(image.data()), 
            {channels, height, width}, 
            torch::TensorOptions().dtype(torch::CppTypeToScalarType<unsigned char>::value).device(torch::kCPU)
            );
        
        // 2. clone to ensure independent memory ownership:
        auto tensorClone = tensor.clone().to(torch::kFloat32).div_(255.0);
            
        // 2.5 [Rows x Cols x Channels] to [Channels x Rows x Cols]
        //tensor = tensor.permute({2, 0, 1}).contiguous().clone();
        
        
        
        // 3. Add batch dimension
        tensorClone = tensorClone.unsqueeze(0);
        
        std::cout << "Tensor shape: " << tensorClone.sizes() << std::endl;
        
        // classify image with Pytorch model
        
        
        // Display class label
        
        
    }
    catch(const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;    
}
