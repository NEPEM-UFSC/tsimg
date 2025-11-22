#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include "SpiceContent.h"
#include "Image.h"

class SPICE {
public:
    SPICE(const std::string& title, bool debug);
    void generateSPICEFileFromTemplate(const std::string& templateFile, const std::string& outputFile, bool debug);
    std::string generateLabelTags();
    void debugPrint() const;
    
    void addContent(const SpiceContent& content);
    void addImageList(const std::string& tag, std::unique_ptr<ImageList> imageList);
    void addLabels(const std::vector<std::string>& newLabels);
    
    void setTitle(const std::string& newTitle);
    void setAuthorImage(const std::string& imagePath);
    const std::string& getTitle() const;

private:
    std::string title;
    bool debug;
    std::vector<SpiceContent> contents;
    std::map<std::string, std::unique_ptr<ImageList>> imageLists;
    std::vector<std::string> labels;
    std::string authorImageBase64;
};
