#pragma once

#include <string>
#include <vector>
#include <map>

class SPICE {
public:
    SPICE(const std::string& title = "", bool debug = false);
    void addImage(const std::string& imagePath, bool debug = false);
    void addContent(const std::string& tag, const std::string& content, bool debug = false);
    void addLabels(const std::vector<std::string>& labelList, bool debug = false);
    void generateSPICEFileFromTemplate(const std::string& templateFile, const std::string& outputFile, bool debug = false);
    void generateLabelsFromImages(bool debug = false);
    void setAuthorImage(const std::string& imagePath, bool debug = false);
    void setHelp(const std::string& helpText, const std::string& helpLink, const std::string& helpBadgeURL, bool debug = false);

private:
    std::string title;
    std::vector<std::string> base64Images;
    std::string sliderControls;
    std::vector<std::string> contentElements;
    std::vector<std::string> labels;
    std::map<std::string, std::string> contentMap;
    std::string authorImageBase64;

    std::string generateImageTags();
    std::string generateLabelTags();
};

std::string replaceTag(const std::string& source, const std::string& tag, const std::string& replacement);
