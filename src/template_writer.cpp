#include "template_writer.h"
#include "image_utils.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <filesystem>

TemplateWriter::TemplateWriter(const std::string& templateFile, bool debug) : debug(debug) {
    templateContent = readFileToString(templateFile);
    if (debug) {
        std::cout << "Template content loaded from: " << templateFile << std::endl;
    }
}

void TemplateWriter::writeToFile(const std::string& outputFile, const std::vector<SpiceContent>& contents, const ImageList& imageList, const std::vector<std::string>& labels, const std::string& authorImageBase64) {
    if (!validateImageListAndLabels(imageList, labels)) {
        if (debug) {
            std::cerr << "Validation failed: The number of images and labels must be the same." << std::endl;
        }
        return;
    }

    std::string outputContent = replaceAllTags(templateContent, contents);

    std::string imageContent = !imageList.getImages().empty() ? imageList.generateImageTags() : "<p>No images available</p>";
    outputContent = replaceTag(outputContent, "<SPICE_IMAGES>", imageContent);

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
    const auto& imageList = builder.getImageList();
    const auto& labels = builder.getLabels();
    const auto& authorImageBase64 = builder.getAuthorImageBase64();

    if (!validateImageListAndLabels(imageList, labels)) {
        if (debug) {
            std::cerr << "Validation failed: The number of images and labels must be the same." << std::endl;
        }
        return;
    }

    std::string outputContent = replaceAllTags(templateContent, contents);

    std::string imageContent = !imageList.getImages().empty() ? imageList.generateImageTags() : "<p>No images available</p>";
    outputContent = replaceTag(outputContent, "<SPICE_IMAGES>", imageContent);

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
    std::ifstream file(filePath);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::string TemplateWriter::replaceTag(const std::string& source, const std::string& tag, const std::string& replacement) {
    std::string result = source;
    size_t pos = result.find(tag);
    while (pos != std::string::npos) {
        result.replace(pos, tag.length(), replacement);
        pos = result.find(tag, pos + replacement.length());
    }
    return result;
}

bool TemplateWriter::validateImageListAndLabels(const ImageList& imageList, const std::vector<std::string>& labels) {
    return imageList.getImages().size() == labels.size();
}

std::string TemplateWriter::replaceAllTags(const std::string& source, const std::vector<SpiceContent>& contents) {
    std::string result = source;
    for (const auto& content : contents) {
        result = replaceTag(result, "<" + content.getTag() + ">", content.getVariableContent());
    }
    return result;
}
