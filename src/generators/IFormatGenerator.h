#pragma once
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

namespace tsimg::generators {
    class IFormatGenerator {
    public:
        virtual ~IFormatGenerator() = default;
        virtual bool generate(const std::string& outputFilename, 
                              const std::vector<std::string>& imagePaths,
                              const std::vector<std::string>& labels,
                              const std::string& title,
                              const nlohmann::json& config) = 0;
    };
}
