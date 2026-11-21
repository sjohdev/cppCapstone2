module;

#include <torch/torch.h>
#include <torch/script.h>
#include "CImg.h"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <exception>
#include <cmath>
#include <stdexcept>

module imageprocessing;

// Functions for reading image files and classifying them using a pre-trained Neural Network. 
// NOTE: These functions was developed with the help of AI (chatGPT-6 Luna) 

namespace ImageProc {

    // Rotate a 2D image clockwise by 90 degrees.
    template <typename T>
    cimg_library::CImg<T> rotate90CW(const cimg_library::CImg<T>& src)
    {
        cimg_library::CImg<T> dst(
            src.height(),
            src.width(),
            src.depth(),
            src.spectrum()
        );

        cimg_forXYZC(src, x, y, z, c) {
            dst(src.height() - 1 - y, x, z, c) =
                src(x, y, z, c);
        }
        return dst;
    }

    // Preprocess image to 192 x 256.
    cimg_library::CImg<unsigned char> preprocessImage(const cimg_library::CImg<unsigned char>& input)
    {
        if (input.is_empty())
            throw std::runtime_error("Empty input image");

        cimg_library::CImg<unsigned char> img(input);

        // 1. Rotate landscape images to portrait.
        if (img.width() > img.height()) {
            img = rotate90CW(img);
        }

        // 2. Center crop to a 3:4 width-to-height ratio.
        constexpr double targetRatio = 192.0 / 256.0;

        const int width = img.width();
        const int height = img.height();

        int cropWidth = width;
        int cropHeight = height;

        if (static_cast<double>(width) / height > targetRatio) {
            cropWidth = static_cast<int>(
                std::round(height * targetRatio)
            );
        } else {
            cropHeight = static_cast<int>(
                std::round(width / targetRatio)
            );
        }

        const int x0 = (width - cropWidth) / 2;
        const int y0 = (height - cropHeight) / 2;

        img = img.get_crop(
            x0, y0,
            x0 + cropWidth - 1,
            y0 + cropHeight - 1
        );

        // 3. Gaussian prefilter for anti-aliasing.
        const double scaleX = 192.0 / img.width();
        const double scaleY = 256.0 / img.height();

        if (scaleX < 1.0 || scaleY < 1.0) {
            const float sigmaX = scaleX < 1.0
                ? static_cast<float>(
                    0.5 * std::sqrt(1.0 / (scaleX * scaleX) - 1.0)
                  )
                : 0.0f;

            const float sigmaY = scaleY < 1.0
                ? static_cast<float>(
                    0.5 * std::sqrt(1.0 / (scaleY * scaleY) - 1.0)
                  )
                : 0.0f;

            img.blur(sigmaX, sigmaY, 0.0f);
        }

        // 4. Resize using bilinear interpolation.
        img.resize(192, 256, 1, -100, 1);

        return img;
    }
    
    void printLabelMap(const std::map<std::string, int>& map) {
        std::cout << "\nImage classification complete. Predicted image-labels: \n" << std::endl;
        for (const auto& [key, value] : map) {
            // Display class label
            std::cout << key << ": " << value << std::endl;
        }
    }

    torch::Tensor CImg2tensor(const cimg_library::CImg<unsigned char>& image) {
        // 1. create tensor
        const int64_t cols = image.width();
        const int64_t rows = image.height();
        const int64_t channels = image.spectrum();
        
        auto tensor = torch::from_blob(
            const_cast<unsigned char*>(image.data()), 
            {channels, rows, cols}, 
            torch::TensorOptions()
            .dtype(torch::CppTypeToScalarType<unsigned char>::value)
            .device(torch::kCPU)
            );
        
        // 2. clone to ensure independent memory ownership:
        torch::Tensor tensorClone = tensor.clone().to(torch::kFloat32).div_(255.0);
            
        // 2.5 [Rows x Cols x Channels] to [Channels x Rows x Cols]
        //tensorClone = tensorClone.permute({2, 0, 1}).contiguous().clone();
        
        // 3. Add batch dimension
        tensorClone = tensorClone.unsqueeze(0);
        
        std::cout << "Tensor shape: " << tensorClone.sizes() << std::endl;
        return tensorClone;
    }

    void processFiles(const std::vector<std::string>& filePathList) {
        std::cout << "Processing Image-files..." << std::endl;
        
        // Initialize variables: 
        std::map<std::string, int> labelmap;
        const std::string modelName = "../mobilenetv3_large_Pretrained.pt";
        torch::Device device (torch::kCPU);
        
        // load PyTorch image-classifier model
        torch::jit::script::Module model;
        model = torch::jit::load(modelName.c_str(), device);
        model.eval();
        
        // Disable LibTorch gradient computation
        torch::NoGradGuard no_grad;
        
        // for each file in filepathlist: 
        for (const std::string& filename : filePathList) {
            try {
                // Read image: Width = columns, Height = rows
                // Load an image, transform it to a color image (if necessary)
                cimg_library::CImg<unsigned char> image(filename.c_str());
                
                // Preprocess image before running image classifier: 
                cimg_library::CImg<unsigned char> processedImage = preprocessImage(image);
                
                // Convert image to tensor
                torch::Tensor input = CImg2tensor(processedImage);
                
                // classify image with Pytorch model
                torch::Tensor output = model.forward({input}).toTensor();
                
                // Get predicted image-label
                torch::Tensor pred = output.argmax(1);
                
                // Add image-label to map
                labelmap[filename] = pred.item<int>();
            }
            catch(const std::exception& e) {
                std::cerr << "Error: " << e.what() << '\n';
                labelmap.clear();
                return;
            }
        }
        
        // Display Classification results: 
        printLabelMap(labelmap);
        labelmap.clear();
    }
    
}

