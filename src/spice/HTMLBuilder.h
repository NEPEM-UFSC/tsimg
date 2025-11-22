#pragma once
#include <string>
#include <vector>

namespace tsimg::utils {
    class HTMLBuilder {
    public:
        static std::string createHelpSection(
            const std::string& helpText, 
            const std::string& helpContent, 
            const std::string& helpLink
        );
        static std::string createLabelTags(const std::vector<std::string>& labels);
    };
}
