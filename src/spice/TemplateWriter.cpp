#include "TemplateWriter.h"
#include "SPICEBuilder.h"
#include "../utils/FileHandler.h"
#include "../utils/Logger.h"
#include "HTMLBuilder.h"
#include "../build_info.h"
#include <iostream>
#include <sstream>
#include <filesystem>
#include <chrono>
#include <iomanip>

// Helper function to get template content (moved from tsimg_spice.cpp)
namespace tsimg::utils {
    std::string getTemplateContent(const std::string& templatePath, bool debug) {
        std::string fullPath = templatePath;
        if (templatePath.empty()) {
            fullPath = TemplateWriter::getDefaultTemplatePath();
        }
        return FileHandler::readFile(fullPath, debug);
    }
}

TemplateWriter::TemplateWriter(const std::string& templatePath, bool debug) : debug(debug) {
    this->templatePath = resolveTemplatePath(templatePath);
    
    if (debug) {
        std::cout << "Resolved template path: " << this->templatePath << std::endl;
    }

    templateContent = tsimg::utils::getTemplateContent(this->templatePath, debug);
    
    if (debug) {
        std::cout << "Using template: " << this->templatePath << std::endl;
    }
}

void TemplateWriter::writeToFile(const std::string& outputFile, 
                                 const std::vector<SpiceContent>& contents, 
                                 const std::map<std::string, std::unique_ptr<ImageList>>& imageLists, 
                                 const std::vector<std::string>& labels, 
                                 const std::string& authorImageBase64) {
    tsimg::utils::debugLog(debug, "Starting writeToFile process for: " + outputFile);
    tsimg::utils::debugLog(debug, "Using template: " + (templatePath.empty() ? getDefaultTemplatePath() : templatePath));

    try {
        if (contents.empty()) {
            throw std::runtime_error("No contents available to write");
        }
        if (imageLists.empty()) {
            throw std::runtime_error("No image lists available to write");
        }
        if (!validateImageListAndLabels(imageLists, labels)) {
            throw std::runtime_error("Image list and labels validation failed - counts must match");
        }

        std::string outputContent = templateContent;
        tsimg::utils::debugLog(debug, "Processing template content...");
        
        outputContent = replaceAllTags(outputContent, contents);
        tsimg::utils::debugLog(debug, "Tags replacement completed");
        
        outputContent = replaceObjectPlaceholders(outputContent, imageLists);
        tsimg::utils::debugLog(debug, "Object placeholders replacement completed");

        std::string authorImageTag = authorImageBase64.empty() ? "" : "data:image/png;base64," + authorImageBase64;
        outputContent = replaceTag(outputContent, "<SPICE_AUTHOR_IMAGE>", authorImageTag);

        if (authorImageTag.empty()) {
            outputContent = replaceTag(outputContent, "<SPICE_AUTHOR_IMAGE>", "");
        }

        std::string labelTags;
        for (const auto& label : labels) {
            labelTags += "<span>" + label + "</span>";
        }
        outputContent = replaceTag(outputContent, "<SPICE_LABELS>", labelTags);

        std::string helpText = "";
        std::string helpLink = "";
        std::string helpBadgeUrl = "";

        for (const auto& content : contents) {
            if (content.getTag() == "SPICE_HELP_TEXT") {
                helpText = content.getVariableContent();
            } else if (content.getTag() == "SPICE_HELP_CONTENT") {
                helpBadgeUrl = content.getVariableContent();
            } else if (content.getTag() == "SPICE_HELP_LINK") {
                helpLink = content.getVariableContent();
            }
        }

        std::string helpSection = tsimg::utils::HTMLBuilder::createHelpSection(
            helpText,
            helpBadgeUrl,
            helpLink
        );

        if (helpSection.empty()) {
            size_t startPos = outputContent.find("<div class=\"help-section\">");
            if (startPos != std::string::npos) {
                size_t endPos = outputContent.find("</div>", startPos);
                if (endPos != std::string::npos) {
                    endPos += 6;
                    outputContent.erase(startPos, endPos - startPos);
                }
            }
            outputContent = replaceTag(outputContent, "<SPICE_HELP_SECTION>", "");
        } else {
            outputContent = replaceTag(outputContent, "<SPICE_HELP_SECTION>", helpSection);
        }

        tsimg::utils::FileHandler::writeFile(outputFile, outputContent, debug);
        tsimg::utils::debugLog(debug, "File written successfully: " + outputFile);

    } catch (const std::exception& e) {
        tsimg::utils::errorLog(debug, std::string("Error writing to file: ") + e.what());
        throw;
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
    outputContent = replaceTag(outputContent, "<SPICE_AUTHOR_IMAGE>", authorImageBase64);

    tsimg::utils::FileHandler::writeFile(outputFile, outputContent, debug);

    if (debug) {
        std::cout << "Output file written to: " << outputFile << std::endl;
    }
}

std::string TemplateWriter::readFileToString(const std::string& filePath) {
    return tsimg::utils::FileHandler::readFile(filePath, debug);
}

std::string TemplateWriter::replaceTag(const std::string& source, const std::string& tag, const std::string& replacement) {
    try {
        if (tag.empty()) {
            throw std::invalid_argument("Empty tag provided");
        }

        std::string result = source;
        size_t pos = result.find(tag);
        
        if (pos == std::string::npos) {
            tsimg::utils::debugLog(debug, "Tag not found in template: " + tag);
            return result;
        }

        while (pos != std::string::npos) {
            result.replace(pos, tag.length(), replacement);
            pos = result.find(tag, pos + replacement.length());
        }

        tsimg::utils::debugLog(debug, "Tag replaced successfully: " + tag);
        return result;
        
    } catch (const std::exception& e) {
        tsimg::utils::errorLog(debug, "Error replacing tag " + tag + ": " + std::string(e.what()));
        throw;
    }
}

bool TemplateWriter::validateImageListAndLabels(const std::map<std::string, std::unique_ptr<ImageList>>& imageLists, const std::vector<std::string>& labels) {
    if (labels.empty()) {
        return true;
    }
    
    auto mainList = imageLists.find("SPICE_IMAGES");
    if (mainList != imageLists.end()) {
        return mainList->second->getImages().size() == labels.size();
    }
    
    if (!imageLists.empty()) {
        for (const auto& [tag, list] : imageLists) {
            if (list->getImages().size() == labels.size()) {
                return true;
            }
        }
        return false;
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

std::string TemplateWriter::replaceObjectPlaceholders(const std::string& source, const std::map<std::string, std::unique_ptr<ImageList>>& imageLists) {
    std::string result = source;
    for (const auto& [tag, imageList] : imageLists) {
        std::string formattedTag = tag;
        if (tag.substr(0, 6) != "SPICE_") {
            formattedTag = "SPICE_" + tag;
        }
        
        std::string placeholder = "<" + formattedTag + ">";
        std::string replacement = imageList->generateImageTags();
        result = replaceTag(result, placeholder, replacement);
    }
    return result;
}

std::string TemplateWriter::buildHtmlStructure(const SPICEBuilder& builder) {
    std::ifstream file(templatePath);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open template file: " + templatePath);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string htmlContent = buffer.str();

    const auto& imageLists = builder.getImageLists();
    std::string multiListsBlock;
    for (const auto& [tag, list] : imageLists) {
        multiListsBlock += 
            "<div class=\"multi-list\">\n"
            "  <div class=\"slider-container\">\n"
            "    <!-- Slider e controles reaproveitados do template principal -->\n"
            "  </div>\n"
            "  <div class=\"slider-images\" data-list=\"" + tag + "\">\n"
            "    <" + tag + ">\n"
            "  </div>\n"
            "  <div class=\"slider-labels\">\n"
            "    <SPICE_LABELS>\n"
            "  </div>\n"
            "</div>\n";
    }

    size_t pos = htmlContent.find("<SPICE_SLIDER_LISTS>");
    if (pos != std::string::npos) {
        htmlContent.replace(pos, std::string("<SPICE_SLIDER_LISTS>").size(), multiListsBlock);
    }

    htmlContent = replaceTag(htmlContent, "<SPICE_TITLE>", builder.getTitle());
    try {
        htmlContent = replaceTag(htmlContent, "<SPICE_TEXT>", 
            builder.generateLabelTags());
    } catch(...) {}

    try {
        htmlContent = replaceTag(htmlContent, "<SPICE_AUTHOR_IMAGE>", builder.getAuthorImageBase64());
    } catch(...) {}

    for (const auto& [tag, list] : imageLists) {
        std::string placeholder = "<" + tag + ">";
        if (htmlContent.find(placeholder) != std::string::npos) {
            std::string imageContent = list->generateImageTags();
            htmlContent = replaceTag(htmlContent, placeholder, imageContent);
        }
    }

    const auto& contents = builder.getContents();
    for (const auto& c : contents) {
        std::string placeholder = "<" + c.getTag() + ">";
        try {
            htmlContent = replaceTag(htmlContent, placeholder, c.getVariableContent());
        } catch(...) {}
    }

    return htmlContent;
}

std::string TemplateWriter::getDefaultTemplatePath() {
    std::filesystem::path defaultPath = std::filesystem::current_path() / "templates" / "base_template.html";
    return defaultPath.string();
}

std::string TemplateWriter::resolveTemplatePath(const std::string& templateName) {
    if (templateName.find(".html") != std::string::npos) {
        return templateName;
    }

    std::filesystem::path templatesPath = std::filesystem::current_path() / "templates";
    std::filesystem::path templatePath = templatesPath / (templateName + ".html");

    if (std::filesystem::exists(templatePath)) {
        return templatePath.string();
    }

    return getDefaultTemplatePath();
}

std::string TemplateWriter::getCurrentDateTime() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string TemplateWriter::formatBuildInfo(const std::string& buildInfo) const {
    std::vector<std::string> lines;
    std::istringstream stream(buildInfo);
    std::string line;
    
    while (std::getline(stream, line)) {
        lines.push_back("    " + line);
    }
    
    std::ostringstream formatted;
    for (const auto& l : lines) {
        formatted << l << "\n";
    }
    
    return formatted.str();
}

std::string TemplateWriter::generateBuildInfo() const {
    std::ostringstream info;
    info << "<!--\n"
         << "    TSIMG Build Information\n"
         << "    ----------------------\n"
         << "    Generated on: " << getCurrentDateTime() << "\n"
         << "    TSIMG Version: " << tsimg::getVersion() << "\n"
         << "    Generator Information:\n";
    
    info << formatBuildInfo(tsimg::getBuildInfo());

    info << "    ----------------------\n"
         << "    Generated by TSIMG (github.com/NEPEM-UFSC/tsimg)\n"
         << "-->";
    
    return info.str();
}
