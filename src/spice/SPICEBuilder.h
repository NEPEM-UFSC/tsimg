#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include "SpiceContent.h"
#include "Image.h"

class SPICEBuilder {
public:
    SPICEBuilder();
    SPICEBuilder(const std::string& title, bool debug);
    
    SPICEBuilder& addImage(const std::string& imagePath);
    SPICEBuilder& addImageToList(const std::string& listTag, const std::string& imagePath);
    SPICEBuilder& addContent(const std::string& tag, const std::string& content);
    SPICEBuilder& addLabels(const std::vector<std::string>& labelList);
    SPICEBuilder& generateLabelsFromImages();
    SPICEBuilder& setAuthorImage(const std::string& imagePath);
    SPICEBuilder& setHelp(const std::string& helpText, const std::string& helpLink, const std::string& helpBadgeURL);
    SPICEBuilder& addTitle(const std::string& title);
    SPICEBuilder& addImagesAsync(const std::vector<std::string>& imagePaths);
    SPICEBuilder& setTemplate(const std::string& templatePath);
    
    const std::vector<SpiceContent>& getContents() const;
    const std::map<std::string, std::unique_ptr<ImageList>>& getImageLists() const;
    const std::vector<std::string>& getLabels() const;
    const std::string& getAuthorImageBase64() const;
    const std::string& getTitle() const;
    const std::string& getTemplatePath() const;
    
    std::string getImageTags() const;
    std::string generateImageTags() const;
    std::string generateLabelTags() const;
    
    void debugPrint() const;
    bool hasAdditionalImages() const;

private:
    std::string title;
    bool debug;
    std::vector<SpiceContent> contents;
    std::map<std::string, std::unique_ptr<ImageList>> imageLists;
    std::vector<std::string> labels;
    std::string authorImageBase64;
    std::string templatePath;
};
