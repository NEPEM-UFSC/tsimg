#include "SPICE.h"
#include "TemplateWriter.h"
#include "../utils/Logger.h"
#include "../utils/FileIO.h"
#include "../utils/Base64.h"
#include <iostream>

SPICE::SPICE(const std::string& title, bool debug)
    : title(title), debug(debug) {}

void SPICE::generateSPICEFileFromTemplate(const std::string& templateFile, const std::string& outputFile, bool debug) {
    if (contents.empty()) {
        if (debug) std::cerr << "Error: No contents available to generate the SPICE file." << std::endl;
        return;
    }
    if (imageLists.empty()) {
        if (debug) std::cerr << "Error: No image lists available to generate the SPICE file." << std::endl;
        return;
    }
    TemplateWriter writer(templateFile, debug);
    writer.writeToFile(outputFile, contents, imageLists, labels, authorImageBase64);
    if (debug) std::cout << "SPICE file generated successfully: " << outputFile << std::endl;
}

std::string SPICE::generateLabelTags() {
    std::string labelTags;
    for (const auto& label : labels) {
        labelTags += "<span>" + label + "</span>";
    }
    return labelTags;
}

void SPICE::debugPrint() const {
    std::cout << "SPICE Debug Info:" << std::endl;
    std::cout << "Title: " << title << std::endl;
    std::cout << "Contents: " << contents.size() << std::endl;
    std::cout << "Image Lists: " << imageLists.size() << std::endl;
    std::cout << "Labels: " << labels.size() << std::endl;
    std::cout << "Author Image Base64: " << (authorImageBase64.empty() ? "Not Set" : "Set") << std::endl;
}

void SPICE::addContent(const SpiceContent& content) {
    contents.push_back(content);
}

void SPICE::addImageList(const std::string& tag, std::unique_ptr<ImageList> imageList) {
    imageLists[tag] = std::move(imageList);
}

void SPICE::addLabels(const std::vector<std::string>& newLabels) {
    labels.insert(labels.end(), newLabels.begin(), newLabels.end());
}

void SPICE::setTitle(const std::string& newTitle) {
    title = newTitle;
}

void SPICE::setAuthorImage(const std::string& imagePath) {
    try {
        authorImageBase64 = tsimg::utils::Base64::encodeFromFile(imagePath);
    } catch (const std::exception& e) {
        tsimg::utils::errorLog(debug, "Error setting author image: " + std::string(e.what()));
    }
}

const std::string& SPICE::getTitle() const {
    return title;
}
