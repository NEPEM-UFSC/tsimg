#pragma once
#include <string>
#include <vector>
#include <future>
#include <memory>
#include "../spice/Image.h"

namespace tsimg::utils {
    class ImageProcessor {
    public:
        static std::vector<std::future<std::unique_ptr<Image>>> processImagesAsync(
            const std::vector<std::string>& imagePaths, bool debug);
        
        static std::unique_ptr<Image> processImage(const std::string& imagePath, bool debug);
        static std::unique_ptr<Image> resizeImage(const std::string& imagePath, int width, int height, bool debug);
        static std::unique_ptr<Image> cropImage(const std::string& imagePath, int x, int y, int width, int height, bool debug);
        static std::unique_ptr<Image> rotateImage(const std::string& imagePath, float angle, bool debug);
    };
}
