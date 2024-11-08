#pragma once

#include <string>
#include <vector>
#include "spice_object.h"

class TemplateWriter {
public:
    TemplateWriter(const std::string& templateFile, bool debug = false);
    void writeToFile(const std::string& outputFile, const std::vector<SpiceContent>& contents, const ImageList& imageList, const std::vector<std::string>& labels, const std::string& authorImageBase64);
    void build(const SPICEBuilder& builder, const std::string& outputFile);

private:
    std::string templateContent;
    bool debug;

    std::string readFileToString(const std::string& filePath);
    std::string replaceTag(const std::string& source, const std::string& tag, const std::string& replacement);
    bool validateImageListAndLabels(const ImageList& imageList, const std::vector<std::string>& labels);
    std::string replaceAllTags(const std::string& source, const std::vector<SpiceContent>& contents);
};
