#include "SPICEBuilder.h"
#include "../utils/Logger.h"
#include "../utils/ImageProcessor.h"
#include "../utils/FileIO.h"
#include "../utils/Base64.h"
#include <iostream>

SPICEBuilder::SPICEBuilder()
    : title(""), debug(false) {}

SPICEBuilder::SPICEBuilder(const std::string& title, bool debug)
    : title(title), debug(debug) {
    if (debug) {
        std::cout << "SPICEBuilder object created with title: " << title << std::endl;
    }
}

SPICEBuilder& SPICEBuilder::addImage(const std::string& imagePath) {
    return addImagesAsync({imagePath});
}

SPICEBuilder& SPICEBuilder::addImagesAsync(const std::vector<std::string>& imagePaths) {
    tsimg::utils::debugLog(debug, "Adding images asynchronously");
    
    auto futures = tsimg::utils::ImageProcessor::processImagesAsync(imagePaths, debug);
    
    for (auto& future : futures) {
        try {
            auto img = future.get();
            if (img && !img->toBase64().empty()) { 
                if (imageLists.find("SPICE_IMAGES") == imageLists.end()) {
                    imageLists["SPICE_IMAGES"] = std::make_unique<ImageList>();
                }
                imageLists["SPICE_IMAGES"]->addImage(std::move(img));
            } else if (img) {
                tsimg::utils::errorLog(debug, "Failed to add image, Base64 data is empty for: " + img->getPath());
            } else {
                tsimg::utils::errorLog(debug, "Failed to process an image, future returned null.");
            }
        } catch (const std::exception& e) {
            tsimg::utils::errorLog(debug, "Failed to add image: " + std::string(e.what()));
        }
    }
    
    return *this;
}

SPICEBuilder& SPICEBuilder::addImageToList(const std::string& listTag, const std::string& imagePath) {
    if (debug) std::cout << "Adding image to " << listTag << ": " << imagePath << std::endl;
    std::string base64Image;
    try {
        base64Image = tsimg::utils::Base64::encodeFromFile(imagePath);
    } catch (const std::exception& e) {
        tsimg::utils::errorLog(debug, "Error encoding image: " + std::string(e.what()));
    }
    
    if (!base64Image.empty()) {
        if (imageLists.find(listTag) == imageLists.end()) {
            imageLists[listTag] = std::make_unique<ImageList>();
        }
        imageLists[listTag]->addImage(std::make_unique<Image>(imagePath, base64Image));
        if (debug) std::cout << "Image added successfully to " << listTag << ": " << imagePath << std::endl;
    } else {
        if (debug) std::cerr << "Failed to add image to " << listTag << ": " << imagePath << std::endl;
    }
    return *this;
}

SPICEBuilder& SPICEBuilder::addContent(const std::string& tag, const std::string& content) {
    if (debug) std::cout << "Adding content to tag <" << tag << ">: " << content << std::endl;
    
    contents.push_back(SpiceContent(tag, content));
    
    if (debug) std::cout << "Content added successfully." << std::endl;
    return *this;
}

SPICEBuilder& SPICEBuilder::addLabels(const std::vector<std::string>& labelList) {
    if (debug) std::cout << "Adding labels: " << std::endl;
    for (const auto& label : labelList) {
        if (debug) std::cout << label << std::endl;
        labels.push_back(label);
    }
    return *this;
}

SPICEBuilder& SPICEBuilder::generateLabelsFromImages() {
    if (debug) std::cout << "Generating labels from images." << std::endl;
    for (const auto& imageList : imageLists) {
        for (const auto& image : imageList.second->getImages()) {
            labels.push_back(image->getPath());
        }
    }
    return *this;
}

SPICEBuilder& SPICEBuilder::setAuthorImage(const std::string& imagePath) {
    if (debug) std::cout << "Setting author image: " << imagePath << std::endl;
    if (authorImageBase64.empty()) {
        try {
            authorImageBase64 = tsimg::utils::Base64::encodeFromFile(imagePath);
            if (debug) std::cout << "Author image set successfully." << std::endl;
        } catch (const std::exception& e) {
            tsimg::utils::errorLog(debug, "Error setting author image: " + std::string(e.what()));
        }
    } else {
        if (debug) std::cout << "Author image already set." << std::endl;
    }
    return *this;
}

SPICEBuilder& SPICEBuilder::setHelp(const std::string& helpText, const std::string& helpLink, const std::string& helpBadgeURL) {
    contents.push_back(SpiceContent("SPICE_HELP_TEXT", helpText));
    if (debug) std::cout << "Help text set successfully." << std::endl;

    std::string helpContentString = "<a href=\"" + helpLink + "\"><img src=\"" + helpBadgeURL + "\" alt=\"Help Badge\"></a>";
    contents.push_back(SpiceContent("SPICE_HELP_CONTENT", helpContentString));
    if (debug) std::cout << "Help content set successfully." << std::endl;
    return *this;
}

SPICEBuilder& SPICEBuilder::addTitle(const std::string& title) {
    if (debug) std::cout << "Adding title: " << title << std::endl;
    contents.push_back(SpiceContent("SPICE_TITLE", title));
    if (debug) std::cout << "Title added successfully." << std::endl;
    return *this;
}

const std::vector<SpiceContent>& SPICEBuilder::getContents() const {
    return contents;
}

const std::map<std::string, std::unique_ptr<ImageList>>& SPICEBuilder::getImageLists() const {
    return imageLists;
}

const std::vector<std::string>& SPICEBuilder::getLabels() const {
    return labels;
}

const std::string& SPICEBuilder::getAuthorImageBase64() const {
    return authorImageBase64;
}

const std::string& SPICEBuilder::getTitle() const {
    return title;
}

std::string SPICEBuilder::generateImageTags() const {
    std::string imageTags;
    for (const auto& imageList : imageLists) {
        imageTags += imageList.second->generateImageTags();
    }
    return imageTags;
}

std::string SPICEBuilder::generateLabelTags() const {
    std::string labelTags;
    for (const auto& label : labels) {
        labelTags += "<span>" + label + "</span>";
    }
    return labelTags;
}

void SPICEBuilder::debugPrint() const {
    std::cout << "SPICEBuilder Debug Info:" << std::endl;
    std::cout << "Title: " << title << std::endl;
    std::cout << "Contents: " << contents.size() << std::endl;
    std::cout << "Image Lists: " << imageLists.size() << std::endl;
    std::cout << "Labels: " << labels.size() << std::endl;
    std::cout << "Author Image Base64: " << (authorImageBase64.empty() ? "Not Set" : "Set") << std::endl;
}

bool SPICEBuilder::hasAdditionalImages() const {
    auto it = imageLists.find("SPICE_IMAGES_1");
    return it != imageLists.end() && !it->second->getImages().empty();
}

SPICEBuilder& SPICEBuilder::setTemplate(const std::string& templatePath) {
    this->templatePath = templatePath;
    return *this;
}

const std::string& SPICEBuilder::getTemplatePath() const {
    return templatePath;
}
