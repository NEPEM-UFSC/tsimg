#pragma once
#include "IFormatGenerator.h"
#include "../tsimg_spice.h"

namespace tsimg::generators {
    class SPICEGenerator : public IFormatGenerator {
    private:
        bool debug;
    public:
        SPICEGenerator(bool debug);
        bool generate(const std::string& outputFilename, 
                      const std::vector<std::string>& imagePaths,
                      const std::vector<std::string>& labels,
                      const std::string& title,
                      const nlohmann::json& config) override;
    };
}
