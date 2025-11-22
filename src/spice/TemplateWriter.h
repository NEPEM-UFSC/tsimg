#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include "SpiceContent.h"
#include "Image.h"

// Forward declaration
class SPICEBuilder;

class TemplateWriter {
public:
    TemplateWriter(const std::string& templatePath, bool debug);
    
    static std::string getDefaultTemplatePath();
    static std::string resolveTemplatePath(const std::string& templateName);
    
    void writeToFile(const std::string& outputFile, 
                     const std::vector<SpiceContent>& contents, 
                     const std::map<std::string, std::unique_ptr<ImageList>>& imageLists, 
                     const std::vector<std::string>& labels, 
                     const std::string& authorImageBase64);
    
    void build(const SPICEBuilder& builder, const std::string& outputFile);
    std::string buildHtmlStructure(const SPICEBuilder& builder);

private:
    std::string generateBuildInfo() const;
    std::string getCurrentDateTime() const;
    std::string formatBuildInfo(const std::string& buildInfo) const;
    std::string readFileToString(const std::string& filePath);
    std::string replaceTag(const std::string& source, const std::string& tag, const std::string& replacement);
    bool validateImageListAndLabels(const std::map<std::string, std::unique_ptr<ImageList>>& imageLists, const std::vector<std::string>& labels);
    std::string replaceAllTags(const std::string& source, const std::vector<SpiceContent>& contents);
    std::string replaceObjectPlaceholders(const std::string& source, const std::map<std::string, std::unique_ptr<ImageList>>& imageLists);

    std::string templatePath;
    std::string templateContent;
    bool debug;
};
