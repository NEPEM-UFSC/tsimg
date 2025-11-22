#pragma once
#include <string>
#include <vector>

namespace tsimg::utils {
    class ImageValidator {
    public:
        static bool validateImagePath(const std::string& filepath, bool debug = false);
        
        // Novos métodos
        static bool validateImageDimensions(const std::string& filepath, int minWidth, int minHeight);
        static bool validateImageFormat(const std::string& filepath, const std::vector<std::string>& allowedFormats);
        static std::vector<std::string> getSupportedFormats();
    };
}
