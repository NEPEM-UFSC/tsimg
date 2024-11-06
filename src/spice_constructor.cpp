#include "spice_object.h"
#include "image_utils.h"
#include "tsimg.h"
#include "template_writer.h"
#include <fstream>
#include <filesystem>
#include <sstream>
#include <iostream>

// Implementação da classe Image
Image::Image(const std::string& path, const std::string& base64) : path(path), base64(base64) {}

std::string Image::getPath() const {
    return path;
}

std::string Image::getBase64() const {
    return base64;
}

// Implementação da classe ImageList
void ImageList::addImage(const Image& image) {
    images.push_back(image);
}

std::vector<Image> ImageList::getImages() const {
    return images;
}

std::string ImageList::generateImageTags() const {
    std::string imageTags;
    for (const auto& image : images) {
        imageTags += "<img src=\"data:image/png;base64," + image.getBase64() + "\" class=\"active\">";
    }
    return imageTags;
}

// Implementação da classe SpiceContent
SpiceContent::SpiceContent(const std::string& tag, const std::string& baseHtml, const std::string& variableContent)
    : tag(tag), baseHtml(baseHtml), variableContent(variableContent) {}

std::string SpiceContent::getTag() const {
    return tag;
}

std::string SpiceContent::getBaseHtml() const {
    return baseHtml;
}

std::string SpiceContent::getVariableContent() const {
    return variableContent;
}

// Implementação da classe SPICEBuilder
SPICEBuilder::SPICEBuilder(const std::string& title, bool debug) : title(title), debug(debug) {
    if (debug) {
        std::cout << "SPICEBuilder object created with title: " << title << std::endl;
    }
}

SPICEBuilder& SPICEBuilder::addImage(const std::string& imagePath) {
    if (debug) std::cout << "Adding image: " << imagePath << std::endl;
    
    std::string base64Image = encodeImageToBase64(imagePath, debug);
    if (!base64Image.empty()) {
        imageLists["SPICE_IMAGES"].addImage(Image(imagePath, base64Image));
        if (debug) std::cout << "Image added successfully." << std::endl;
    } else {
        if (debug) std::cout << "Failed to add image." << std::endl;
    }
    return *this;
}

SPICEBuilder& SPICEBuilder::addContent(const std::string& tag, const std::string& content) {
    if (debug) std::cout << "Adding content to tag <" << tag << ">: " << content << std::endl;
    
    contents.push_back(SpiceContent(tag, "", content));
    
    if (debug) std::cout << "Content added successfully to tag: " << tag << std::endl;
    return *this;
}

SPICEBuilder& SPICEBuilder::addLabels(const std::vector<std::string>& labelList) {
    if (debug) std::cout << "Adding labels: " << std::endl;
    for (const auto& label : labelList) {
        if (debug) std::cout << "Label: " << label << std::endl;
        labels.push_back(label);
    }
    if (debug) std::cout << "Labels added successfully." << std::endl;
    return *this;
}

SPICEBuilder& SPICEBuilder::generateLabelsFromImages() {
    if (debug) std::cout << "Generating labels from image names..." << std::endl;
    labels.clear();
    for (const auto& image : imageLists["SPICE_IMAGES"].getImages()) {
        std::string label = std::filesystem::path(image.getPath()).stem().string();
        labels.push_back(label);
        if (debug) std::cout << "Generated label: " << label << std::endl;
    }
    if (debug) std::cout << "Labels generated successfully from image names." << std::endl;
    return *this;
}

SPICEBuilder& SPICEBuilder::setAuthorImage(const std::string& imagePath) {
    if (debug) std::cout << "Setting author image: " << imagePath << std::endl;
    
    authorImageBase64 = encodeImageToBase64(imagePath, debug);
    if (authorImageBase64.empty()) {
        if (debug) std::cerr << "Failed to set author image." << std::endl;
    } else {
        if (debug) std::cout << "Author image set successfully." << std::endl;
    }
    return *this;
}

SPICEBuilder& SPICEBuilder::setHelp(const std::string& helpText, const std::string& helpLink, const std::string& helpBadgeURL) {
    contents.push_back(SpiceContent("SPICE_HELP_TEXT", "", helpText));
    if (debug) std::cout << "Help text set successfully." << std::endl;

    std::string helpContent = "<a href=\"" + helpLink + "\"><img src=\"" + helpBadgeURL + "\" alt=\"Help Badge\"></a>";
    contents.push_back(SpiceContent("SPICE_HELP_CONTENT", "", helpContent));
    if (debug) std::cout << "Help content set successfully." << std::endl;
    return *this;
}

SPICEBuilder& SPICEBuilder::addTitle(const std::string& title) {
    if (debug) std::cout << "Adding title: " << title << std::endl;
    contents.push_back(SpiceContent("SPICE_TITLE", "", title));
    if (debug) std::cout << "Title added successfully." << std::endl;
    return *this;
}

const std::vector<SpiceContent>& SPICEBuilder::getContents() const {
    return contents;
}

const std::map<std::string, ImageList>& SPICEBuilder::getImageLists() const {
    return imageLists;
}

const std::vector<std::string>& SPICEBuilder::getLabels() const {
    return labels;
}

const std::string& SPICEBuilder::getAuthorImageBase64() const {
    return authorImageBase64;
}

// Implementação da classe SPICE
SPICE::SPICE(const std::string& title, bool debug) : title(title) {
    if (debug) {
        std::cout << "SPICE object created with title: " << title << std::endl;
    }
}

void SPICE::generateSPICEFileFromTemplate(const std::string& templateFile, const std::string& outputFile, bool debug) {
    TemplateWriter writer(templateFile, debug);
    writer.writeToFile(outputFile, contents, imageLists, labels, authorImageBase64);
}

std::string SPICE::generateLabelTags() {
    std::string labelTags;
    for (const auto& label : labels) {
        labelTags += "<span>" + label + "</span>";
    }
    return labelTags;
}