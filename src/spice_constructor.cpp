#include "spice_object.h"
#include "image_utils.h"
#include "tsimg.h"
#include <fstream>
#include <filesystem>
#include <sstream>
#include <iostream>

SPICE::SPICE(const std::string& title, bool debug) : title(title) {
    if (debug) {
        std::cout << "SPICE object created with title: " << title << std::endl;
    }
}

void SPICE::addImage(const std::string& imagePath, bool debug) {
    if (debug) std::cout << "Adding image: " << imagePath << std::endl;
    
    std::string base64Image = encodeImageToBase64(imagePath, debug);
    if (!base64Image.empty()) {
        base64Images.push_back(base64Image);
        if (debug) std::cout << "Image added successfully." << std::endl;
    } else {
        if (debug) std::cout << "Failed to add image." << std::endl;
    }
}

void SPICE::addContent(const std::string& tag, const std::string& content, bool debug) {
    if (debug) std::cout << "Adding content to tag <" << tag << ">: " << content << std::endl;
    
    contentMap[tag] = content;
    
    if (debug) std::cout << "Content added successfully to tag: " << tag << std::endl;
}

void SPICE::addLabels(const std::vector<std::string>& labelList, bool debug) {
    if (debug) std::cout << "Adding labels: " << std::endl;
    for (const auto& label : labelList) {
        if (debug) std::cout << "Label: " << label << std::endl;
        labels.push_back(label);
    }
    if (debug) std::cout << "Labels added successfully." << std::endl;
}

void SPICE::generateLabelsFromImages(bool debug) {
    if (debug) std::cout << "Generating labels from image names..." << std::endl;
    labels.clear();
    for (const auto& base64Image : base64Images) {
        std::string label = std::filesystem::path(base64Image).stem().string();
        labels.push_back(label);
        if (debug) std::cout << "Generated label: " << label << std::endl;
    }
    if (debug) std::cout << "Labels generated successfully from image names." << std::endl;
}

std::string replaceTag(const std::string& source, const std::string& tag, const std::string& replacement) {
    std::string result = source;
    size_t pos = result.find(tag);
    while (pos != std::string::npos) {
        result.replace(pos, tag.length(), replacement);
        pos = result.find(tag, pos + replacement.length());
    }
    return result;
}

void SPICE::generateSPICEFileFromTemplate(const std::string& templateFile, const std::string& outputFile, bool debug) {
    std::string templateContent = readFileToString(templateFile, debug);

    std::string titleContent = !title.empty() ? title : "SPICE Presentation";

    templateContent = replaceTag(templateContent, "<title><SPICE_TITLE></title>", "<title>" + titleContent + "</title>");
    templateContent = replaceTag(templateContent, "<SPICE_TITLE>", titleContent);

    for (const auto& entry : contentMap) {
        templateContent = replaceTag(templateContent, "<" + entry.first + ">", entry.second);
    }

    std::string imageContent = !base64Images.empty() ? generateImageTags() : "<p>No images available</p>";
    templateContent = replaceTag(templateContent, "<SPICE_IMAGES>", imageContent);

    std::string labelContent = !labels.empty() ? generateLabelTags() : "";
    templateContent = replaceTag(templateContent, "<SPICE_LABELS>", labelContent);

    if (!authorImageBase64.empty()) {
        templateContent = replaceTag(templateContent, "<SPICE_AUTHOR_IMAGE>", "<img src=\"data:image/png;base64," + authorImageBase64 + "\">");
    } else {
        templateContent = replaceTag(templateContent, "<SPICE_AUTHOR_IMAGE>", "");
    }


    std::ofstream outFile(outputFile);
    outFile << templateContent;
    outFile.close();

    if (debug) {
        std::cout << "SPICE file generated successfully: " << outputFile << std::endl;
    }
}

std::string SPICE::generateImageTags() {
    std::string imageTags;
    for (const auto& base64Image : base64Images) {
        imageTags += "<img src=\"data:image/png;base64," + base64Image + "\" class=\"active\">";
    }
    return imageTags;
}

std::string SPICE::generateLabelTags() {
    std::string labelTags;
    for (const auto& label : labels) {
        labelTags += "<span>" + label + "</span>";
    }
    return labelTags;
}

void SPICE::setAuthorImage(const std::string& imagePath, bool debug) {
    if (debug) std::cout << "Setting author image: " << imagePath << std::endl;
    
    authorImageBase64 = encodeImageToBase64(imagePath, debug);
    if (authorImageBase64.empty()) {
        if (debug) std::cerr << "Failed to set author image." << std::endl;
    } else {
        if (debug) std::cout << "Author image set successfully." << std::endl;
    }
}

void SPICE::setHelp(const std::string& helpText, const std::string& helpLink, const std::string& helpBadgeURL, bool debug) {
    this->addContent("SPICE_HELP_TEXT", helpText, debug);
    if (debug) std::cout << "Help text set successfully." << std::endl;

    std::string helpContent = "<a href=\"" + helpLink + "\"><img src=\"" + helpBadgeURL + "\" alt=\"Help Badge\"></a>";
    this->addContent("SPICE_HELP_CONTENT", helpContent, debug);
    if (debug) std::cout << "Help content set successfully." << std::endl;
}