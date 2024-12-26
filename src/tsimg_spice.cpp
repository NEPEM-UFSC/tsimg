#include "tsimg_spice.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <filesystem>
#include <stdexcept>

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

Image::Image(const std::string& path, const std::string& base64)
    : path(path), base64(base64) {}

std::string Image::getPath() const {
    return path;
}

std::string Image::getBase64() const {
    return base64;
}

void ImageList::addImage(const Image& image) {
    images.push_back(image);
}

std::vector<Image> ImageList::getImages() const {
    return images;
}

std::string ImageList::generateImageTags() const {
    std::string imageTags;
    for (const auto& image : images) {
        imageTags += "<img src=\"" + image.getBase64() + "\" alt=\"" + image.getPath() + "\">";
    }
    return imageTags;
}

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

SPICEBuilder::SPICEBuilder()
    : title(""), debug(false) {}

SPICEBuilder::SPICEBuilder(const std::string& title, bool debug)
    : title(title), debug(debug) {
    if (debug) {
        std::cout << "SPICEBuilder object created with title: " << title << std::endl;
    }
}

SPICEBuilder& SPICEBuilder::addImage(const std::string& imagePath) {
    if (debug) std::cout << "Adding image to SPICE_IMAGES: " << imagePath << std::endl;
    std::string base64Image = encodeImageToBase64(imagePath, debug);
    if (!base64Image.empty()) {
        imageLists["SPICE_IMAGES"].addImage(Image(imagePath, base64Image));
        if (debug) std::cout << "Image added successfully: " << imagePath << std::endl;
    } else {
        if (debug) std::cerr << "Failed to add image: " << imagePath << std::endl;
    }
    return *this;
}

SPICEBuilder& SPICEBuilder::addImageToList(const std::string& listTag, const std::string& imagePath) {
    if (debug) std::cout << "Adding image to " << listTag << ": " << imagePath << std::endl;
    std::string base64Image = encodeImageToBase64(imagePath, debug);
    if (!base64Image.empty()) {
        imageLists[listTag].addImage(Image(imagePath, base64Image));
        if (debug) std::cout << "Image added successfully to " << listTag << ": " << imagePath << std::endl;
    } else {
        if (debug) std::cerr << "Failed to add image to " << listTag << ": " << imagePath << std::endl;
    }
    return *this;
}

SPICEBuilder& SPICEBuilder::addContent(const std::string& tag, const std::string& content) {
    if (debug) std::cout << "Adding content to tag <" << tag << ">: " << content << std::endl;
    
    contents.push_back(SpiceContent(tag, "", content));
    
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
        for (const auto& image : imageList.second.getImages()) {
            labels.push_back(image.getPath());
        }
    }
    return *this;
}

SPICEBuilder& SPICEBuilder::setAuthorImage(const std::string& imagePath) {
    if (debug) std::cout << "Setting author image: " << imagePath << std::endl;
    if (authorImageBase64.empty()) {
        authorImageBase64 = encodeImageToBase64(imagePath, debug);
        if (debug) std::cout << "Author image set successfully." << std::endl;
    } else {
        if (debug) std::cout << "Author image already set." << std::endl;
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

const std::string& SPICEBuilder::getTitle() const {
    return title;
}

std::string SPICEBuilder::getImageTags() const {
    std::string imageTags;
    for (const auto& imageList : imageLists) {
        imageTags += imageList.second.generateImageTags();
    }
    return imageTags;
}

std::string SPICEBuilder::generateImageTags() const {
    std::string imageTags;
    for (const auto& imageList : imageLists) {
        imageTags += imageList.second.generateImageTags();
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

TemplateWriter::TemplateWriter(const std::string& templatePath, bool debug) : templatePath(templatePath), debug(debug) {
    templateContent = readFileToString(templatePath);
}

void TemplateWriter::writeToFile(const std::string& outputFile, 
                                 const std::vector<SpiceContent>& contents, 
                                 const std::map<std::string, ImageList>& imageLists, 
                                 const std::vector<std::string>& labels, 
                                 const std::string& authorImageBase64) {
    if (debug) {
        std::cout << "Starting writeToFile with outputFile: " << outputFile << std::endl;
    }

    if (contents.empty()) {
        if (debug) std::cerr << "Error: No contents available to write to the file." << std::endl;
        return;
    }
    if (imageLists.empty()) {
        if (debug) std::cerr << "Error: No image lists available to write to the file." << std::endl;
        return;
    }
    if (!validateImageListAndLabels(imageLists, labels)) {
        if (debug) {
            std::cerr << "Validation failed: The number of images and labels must be the same." << std::endl;
        }
        return;
    }

    std::string outputContent = templateContent;

    // Substituir todas as tags de conteúdo
    outputContent = replaceAllTags(outputContent, contents);

    // Substituir placeholders de objetos (listas de imagens)
    outputContent = replaceObjectPlaceholders(outputContent, imageLists);

    // Substituir a imagem do autor
    outputContent = replaceTag(outputContent, "<SPICE_AUTHOR_IMAGE>", authorImageBase64);

    std::ofstream outFile(outputFile);
    if (!outFile.is_open()) {
        if (debug) std::cerr << "Error: Could not open output file: " << outputFile << std::endl;
        return;
    }
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
    outputContent = replaceTag(outputContent, "<SPICE_AUTHOR_IMAGE>", authorImageBase64);

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
            if (debug) std::cerr << "Placeholder not found for tag: " + tag << std::endl;
            continue;
        }
        std::string imageContent = !imageList.getImages().empty() ? imageList.generateImageTags() : "<p>No images available</p>";
        result = replaceTag(result, "<" + tag + ">", imageContent);
        if (debug) std::cout << "Replaced placeholder for tag: " + tag << std::endl;
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

    // Gera o bloco HTML para cada lista de imagens e substitui <SPICE_SLIDER_LISTS>
    const auto& imageLists = builder.getImageLists();
    std::string multiListsBlock;
    for (const auto& [tag, list] : imageLists) {
        // Cada bloco terá suas próprias <div class="slider-images">
        // mas todos compartilharão o mesmo input range no template
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

    // Replace a região do template
    size_t pos = htmlContent.find("<SPICE_SLIDER_LISTS>");
    if (pos != std::string::npos) {
        htmlContent.replace(pos, std::string("<SPICE_SLIDER_LISTS>").size(), multiListsBlock);
    }

    // Substitui as tags e placeholders padrões
    htmlContent = replaceTag(htmlContent, "<SPICE_TITLE>", builder.getTitle());
    // Texto principal (por exemplo, <SPICE_TEXT>)
    // Caso o usuário adicione um content com tag "SPICE_TEXT"
    // substitui, se existir:
    try {
        htmlContent = replaceTag(htmlContent, "<SPICE_TEXT>", 
            builder.generateLabelTags()); // ajusta se necessário
    } catch(...) {
        // Se não existir a tag <SPICE_TEXT>, ignore
    }

    // Substitui a imagem do autor
    try {
        htmlContent = replaceTag(htmlContent, "<SPICE_AUTHOR_IMAGE>", builder.getAuthorImageBase64());
    } catch(...) {
        // Se não existir a tag, ignore
    }

    // Substituir placeholders de imagens
    for (const auto& [tag, list] : imageLists) {
        std::string placeholder = "<" + tag + ">";
        if (htmlContent.find(placeholder) != std::string::npos) {
            std::string imageContent = list.generateImageTags();
            htmlContent = replaceTag(htmlContent, placeholder, imageContent);
        }
    }

    // Substituir tags de conteúdo (SPICE_HELP_TEXT, etc.)
    const auto& contents = builder.getContents();
    for (const auto& c : contents) {
        std::string placeholder = "<" + c.getTag() + ">";
        try {
            htmlContent = replaceTag(htmlContent, placeholder, c.getVariableContent());
        } catch(...) {
            // Se não existir a tag, ignore
        }
    }

    return htmlContent;
}
