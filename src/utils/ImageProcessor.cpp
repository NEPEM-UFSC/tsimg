#include "ImageProcessor.h"
#include "FileIO.h"
#include "Base64.h"
#include "Logger.h"

namespace tsimg::utils {

    std::vector<std::future<std::unique_ptr<Image>>> ImageProcessor::processImagesAsync(const std::vector<std::string>& imagePaths, bool debug) {
        std::vector<std::future<std::unique_ptr<Image>>> futures;
        for (const auto& path : imagePaths) {
            futures.push_back(std::async(std::launch::async, [path, debug]() {
                try {
                    auto imageData = FileIO::readBinary(path);
                    std::string base64 = Base64::encode(imageData);
                    return std::make_unique<Image>(path, base64);
                } catch (const std::exception& e) {
                    errorLog(debug, "Error processing image: " + path + " - " + e.what());
                    return std::make_unique<Image>(path, "");
                }
            }));
        }
        return futures;
    }

    std::unique_ptr<Image> ImageProcessor::processImage(const std::string& imagePath, bool debug) {
        // Implementação futura
        return nullptr;
    }

    std::unique_ptr<Image> ImageProcessor::resizeImage(const std::string& imagePath, int width, int height, bool debug) {
        // Implementação futura
        return nullptr;
    }

    std::unique_ptr<Image> ImageProcessor::cropImage(const std::string& imagePath, int x, int y, int width, int height, bool debug) {
        // Implementação futura
        return nullptr;
    }

    std::unique_ptr<Image> ImageProcessor::rotateImage(const std::string& imagePath, float angle, bool debug) {
        // Implementação futura
        return nullptr;
    }
}
