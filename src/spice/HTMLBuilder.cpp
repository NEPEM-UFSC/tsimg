#include "HTMLBuilder.h"
#include <sstream>

namespace tsimg::utils {
    std::string HTMLBuilder::createHelpSection(
        const std::string& helpText, 
        const std::string& helpContent, 
        const std::string& helpLink
    ) {
        if (helpText.empty() || helpContent.empty() || helpLink.empty()) {
            return "";
        }

        std::ostringstream oss;
        oss << "<div class=\"help-button-container\">\n";
        oss << "    <div class=\"help-text\">\n"
            << "        " << helpText << "\n"
            << "    </div>\n"
            << "    <div class=\"help-badge\">\n"
            << "        " << helpContent << "\n"
            << "    </div>\n"
            << "</div>";
        return oss.str();
    }

    std::string HTMLBuilder::createLabelTags(const std::vector<std::string>& labels) {
        std::ostringstream oss;
        for (const auto& label : labels) {
            oss << "<span>" << label << "</span>";
        }
        return oss.str();
    }
}
