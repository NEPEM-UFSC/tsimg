#pragma once
#include "IFormatGenerator.h"
#include "../tsimg_gif.h"

namespace tsimg::generators {
    class GIFGenerator : public IFormatGenerator {
    private:
        bool debug;
    public:
        GIFGenerator(bool debug);
        bool generate(const std::string& outputFilename, 
                      const std::vector<std::string>& imagePaths,
                      const std::vector<std::string>& labels,
                      const std::string& title,
                      const nlohmann::json& config) override;
    };
}
