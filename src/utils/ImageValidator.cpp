#include "ImageValidator.h"
#include "FileHandler.h"
#include "Logger.h"
#include <filesystem>

namespace tsimg::utils {

    bool ImageValidator::validateImagePath(const std::string& filepath, bool debug) {
        try {
            if (!std::filesystem::exists(filepath)) {
                errorLog(debug, "File does not exist: " + filepath);
                return false;
            }
            
            if (!FileHandler::isFileReadable(filepath)) {
                errorLog(debug, "File is not readable or permission denied: " + filepath);
                return false;
            }
            
            if (!FileHandler::isValidImageFormat(filepath)) {
                errorLog(debug, "Invalid image format. Supported formats: jpg, jpeg, png, gif, bmp. File: " + filepath);
                return false;
            }
            
            debugLog(debug, "Image validation successful: " + filepath);
            return true;
        } catch (const std::exception& e) {
            errorLog(debug, "Exception during image validation: " + std::string(e.what()) + " for file: " + filepath);
            return false;
        }
    }

    bool ImageValidator::validateImageDimensions(const std::string& filepath, int minWidth, int minHeight) {
        // Implementação futura
        return true;
    }

    bool ImageValidator::validateImageFormat(const std::string& filepath, const std::vector<std::string>& allowedFormats) {
        // Implementação futura
        return true;
    }

    std::vector<std::string> ImageValidator::getSupportedFormats() {
        return {".jpg", ".jpeg", ".png", ".gif", ".bmp"};
    }
}
