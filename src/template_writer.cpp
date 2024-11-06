#include "template_writer.h"
#include "image_utils.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <filesystem>
#include <stdexcept>

TemplateWriter::TemplateWriter(const std::string& templateFile, bool debug) : debug(debug) {
    templateContent = readFileToString(templateFile);
    if (debug) {
        std::cout << "Template content loaded from: " << templateFile << std::endl;
    }
}

void TemplateWriter::writeToFile(const std::string& outputFile, 
                                 const std::vector<SpiceContent>& contents, 
                                 const std::map<std::string, ImageList>& imageLists, 
                                 const std::vector<std::string>& labels, 
                                 const std::string& authorImageBase64) {
    if (!validateImageListAndLabels(imageLists, labels)) {
        if (debug) {
            std::cerr << "Validation failed: The number of images and labels must be the same." << std::endl;
        }
        return;
    }

    std::string outputContent = replaceAllTags(templateContent, contents);
    outputContent = replaceObjectPlaceholders(outputContent, imageLists);

    std::string labelContent;
    for (const auto& label : labels) {
        labelContent += "<span>" + label + "</span>";
    }
    outputContent = replaceTag(outputContent, "<SPICE_LABELS>", labelContent);

    if (!authorImageBase64.empty()) {
        outputContent = replaceTag(outputContent, "<SPICE_AUTHOR_IMAGE>", "<img src=\"data:image/png;base64," + authorImageBase64 + "\">");
    } else {
        outputContent = replaceTag(outputContent, "<SPICE_AUTHOR_IMAGE>", "");
    }

    std::ofstream outFile(outputFile);
    outFile << outputContent;
    outFile.close();

    if (debug) {
        std::cout << "Output file written to: " << outputFile << std::endl;
    }
}

void TemplateWriter::build(const SPICEBuilder& builder, const std::string& outputFile) {
    const auto& contents = builder.getContents();
    const auto& imageLists = builder.getImageLists();
    const auto& labels = builder.getLabels();
    const auto& authorImageBase64 = builder.getAuthorImageBase64();

    if (!validateImageListAndLabels(imageLists, labels)) {
        if (debug) {
            std::cerr << "Validation failed: The number of images and labels must be the same." << std::endl;
        }
        return;
    }

    std::string outputContent = replaceAllTags(templateContent, contents);
    outputContent = replaceObjectPlaceholders(outputContent, imageLists);

    std::string labelContent;
    for (const auto& label : labels) {
        labelContent += "<span>" + label + "</span>";
    }
    outputContent = replaceTag(outputContent, "<SPICE_LABELS>", labelContent);

    if (!authorImageBase64.empty()) {
        outputContent = replaceTag(outputContent, "<SPICE_AUTHOR_IMAGE>", "<img src=\"data:image/png;base64," + authorImageBase64 + "\">");
    } else {
        outputContent = replaceTag(outputContent, "<SPICE_AUTHOR_IMAGE>", "");
    }

    std::ofstream outFile(outputFile);
    outFile << outputContent;
    outFile.close();

    if (debug) {
        std::cout << "Output file written to: " << outputFile << std::endl;
    }
}

std::string TemplateWriter::readFileToString(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::in | std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filePath);
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::string buffer(size, '\0');
    if (!file.read(&buffer[0], size)) {
        throw std::runtime_error("Error reading file: " + filePath);
    }

    return buffer;
}

std::string TemplateWriter::replaceTag(const std::string& source, const std::string& tag, const std::string& replacement) {
    std::string result = source;
    size_t pos = result.find(tag);
    if (pos == std::string::npos) {
        throw std::runtime_error("Placeholder not found: " + tag);
    }
    while (pos != std::string::npos) {
        result.replace(pos, tag.length(), replacement);
        pos = result.find(tag, pos + replacement.length());
    }
    return result;
}

bool TemplateWriter::validateImageListAndLabels(const std::map<std::string, ImageList>& imageLists, const std::vector<std::string>& labels) {
    for (const auto& [tag, imageList] : imageLists) {
        if (imageList.getImages().size() != labels.size()) {
            return false;
        }
    }
    return true;
}

std::string TemplateWriter::replaceAllTags(const std::string& source, const std::vector<SpiceContent>& contents) {
    std::string result = source;
    for (const auto& content : contents) {
        result = replaceTag(result, "<" + content.getTag() + ">", content.getVariableContent());
    }
    return result;
}

std::string TemplateWriter::replaceObjectPlaceholders(const std::string& source, const std::map<std::string, ImageList>& imageLists) {
    std::string result = source;
    for (const auto& [tag, imageList] : imageLists) {
        if (source.find("<" + tag + ">") == std::string::npos) {
            throw std::runtime_error("Placeholder not found for tag: " + tag);
        }
        std::string imageContent = !imageList.getImages().empty() ? imageList.generateImageTags() : "<p>No images available</p>";
        result = replaceTag(result, "<" + tag + ">", imageContent);
    }
    return result;
}
