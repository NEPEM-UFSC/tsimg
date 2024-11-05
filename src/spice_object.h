#pragma once

#include <string>
#include <vector>
#include <map>

class Image {
public:
    Image(const std::string& path, const std::string& base64);
    std::string getPath() const;
    std::string getBase64() const;

private:
    std::string path;
    std::string base64;
};

class ImageList {
public:
    void addImage(const Image& image);
    std::vector<Image> getImages() const;
    std::string generateImageTags() const;

private:
    std::vector<Image> images;
};

class SpiceContent {
public:
    SpiceContent(const std::string& tag, const std::string& baseHtml, const std::string& variableContent);
    std::string getTag() const;
    std::string getBaseHtml() const;
    std::string getVariableContent() const;

private:
    std::string tag;
    std::string baseHtml;
    std::string variableContent;
};

class SPICEBuilder {
public:
    SPICEBuilder(const std::string& title = "", bool debug = false);
    SPICEBuilder& addImage(const std::string& imagePath);
    SPICEBuilder& addContent(const std::string& tag, const std::string& content);
    SPICEBuilder& addLabels(const std::vector<std::string>& labelList);
    SPICEBuilder& generateLabelsFromImages();
    SPICEBuilder& setAuthorImage(const std::string& imagePath);
    SPICEBuilder& setHelp(const std::string& helpText, const std::string& helpLink, const std::string& helpBadgeURL);
    SPICEBuilder& addTitle(const std::string& title);

    const std::vector<SpiceContent>& getContents() const;
    const ImageList& getImageList() const;
    const std::vector<std::string>& getLabels() const;
    const std::string& getAuthorImageBase64() const;

private:
    std::string title;
    bool debug;
    ImageList imageList;
    std::vector<SpiceContent> contents;
    std::vector<std::string> labels;
    std::string authorImageBase64;
};

class SPICE {
public:
    SPICE(const std::string& title = "", bool debug = false);
    void generateSPICEFileFromTemplate(const std::string& templateFile, const std::string& outputFile, bool debug = false);

private:
    std::string title;
    ImageList imageList;
    std::vector<SpiceContent> contents;
    std::vector<std::string> labels;
    std::string authorImageBase64;

    std::string generateLabelTags();
};

std::string replaceTag(const std::string& source, const std::string& tag, const std::string& replacement);
