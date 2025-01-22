#include "tsimg_spice.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <filesystem>
#include <stdexcept>
#include <algorithm>
#include <thread>
#include <future>

namespace tsimg::utils {
    void debugLog(bool debug, const std::string& message) {
        if (debug) std::cout << "[DEBUG] " << message << std::endl;
    }

    void errorLog(bool debug, const std::string& message) {
        if (debug) std::cerr << "[ERROR] " << message << std::endl;
    }

    std::string FileHandler::readFile(const std::string& filepath, bool debug) {
        try {
            validateFilePath(filepath);
            std::ifstream file(filepath, std::ios::in | std::ios::binary | std::ios::ate);
            
            if (!file.is_open()) {
                throw std::runtime_error("Could not open file: " + filepath);
            }

            return readFileContent(file, filepath, debug);
        }
        catch (const std::exception& e) {
            errorLog(debug, std::string("Error reading file: ") + e.what());
            throw;
        }
    }

    void FileHandler::writeFile(const std::string& filepath, const std::string& content, bool debug) {
        try {
            validateFilePath(filepath);
            createDirectoryIfNeeded(filepath);
            
            std::ofstream file(filepath);
            if (!file.is_open()) {
                throw std::runtime_error("Could not open file for writing: " + filepath);
            }

            writeFileContent(file, content);
            debugLog(debug, "File written successfully: " + filepath);
        }
        catch (const std::exception& e) {
            errorLog(debug, std::string("Error writing file: ") + e.what());
            throw;
        }
    }

    bool FileHandler::isValidImageFormat(const std::string& filepath) {
        static const std::vector<std::string> validExtensions = {
            ".jpg", ".jpeg", ".png", ".gif", ".bmp"
        };
        
        std::string ext = getFileExtension(filepath);
        return std::find(validExtensions.begin(), validExtensions.end(), ext) != validExtensions.end();
    }

    bool FileHandler::isFileReadable(const std::string& filepath) {
        try {
            validateFilePath(filepath);
            std::ifstream file(filepath);
            return file.good();
        }
        catch (...) {
            return false;
        }
    }

    void FileHandler::validateFilePath(const std::string& filepath) {
        if (filepath.empty()) {
            throw std::invalid_argument("Empty file path");
        }
    }

    void FileHandler::createDirectoryIfNeeded(const std::string& filepath) {
        auto directory = std::filesystem::path(filepath).parent_path();
        if (!directory.empty() && !std::filesystem::exists(directory)) {
            std::filesystem::create_directories(directory);
        }
    }

    std::string FileHandler::readFileContent(std::ifstream& file, const std::string& filepath, bool debug) {
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        std::string buffer(size, '\0');
        if (!file.read(&buffer[0], size)) {
            throw std::runtime_error("Error reading file content: " + filepath);
        }

        debugLog(debug, "File read successfully: " + filepath);
        return buffer;
    }

    void FileHandler::writeFileContent(std::ofstream& file, const std::string& content) {
        file << content;
        file.close();
        
        if (file.fail()) {
            throw std::runtime_error("Failed to write file content");
        }
    }

    std::string FileHandler::getFileExtension(const std::string& filepath) {
        std::string ext = std::filesystem::path(filepath).extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        return ext;
    }

    bool ImageValidator::validateImagePath(const std::string& filepath, bool debug) {
        try {
            if (!std::filesystem::exists(filepath)) {
                errorLog(debug, "File does not exist: " + filepath);
                return false;
            }
            
            if (!FileHandler::isFileReadable(filepath)) {
                errorLog(debug, "File is not readable or permission denied: " + filepath);
                return false;
            }
            
            if (!FileHandler::isValidImageFormat(filepath)) {
                errorLog(debug, "Invalid image format. Supported formats: jpg, jpeg, png, gif, bmp. File: " + filepath);
                return false;
            }
            
            debugLog(debug, "Image validation successful: " + filepath);
            return true;
        } catch (const std::exception& e) {
            errorLog(debug, "Exception during image validation: " + std::string(e.what()) + " for file: " + filepath);
            return false;
        }
    }

    // HTMLBuilder implementation
    std::string HTMLBuilder::createHelpSection(
        const std::string& helpText, 
        const std::string& helpContent, 
        const std::string& helpLink
    ) {
        if (helpText.empty() || helpContent.empty() || helpLink.empty()) {
            return "";
        }

        std::ostringstream oss;
        oss << "<div class=\"help-button-container\">\n";
        oss << "    <div class=\"help-text\">\n"
            << "        " << helpText << "\n"
            << "    </div>\n"
            << "    <div class=\"help-badge\">\n"
            << "        " << helpContent << "\n"
            << "    </div>\n"
            << "</div>";
        return oss.str();
    }

    std::string HTMLBuilder::createLabelTags(const std::vector<std::string>& labels) {
        std::ostringstream oss;
        for (const auto& label : labels) {
            oss << "<span>" << label << "</span>";
        }
        return oss.str();
    }

    std::string Base64::encode(const std::vector<unsigned char>& data) {
        static const char* encoding_table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        std::string encoded;
        encoded.reserve(((data.size() + 2) / 3) * 4);

        for (size_t i = 0; i < data.size(); i += 3) {
            uint32_t octet_a = i < data.size() ? data[i] : 0;
            uint32_t octet_b = i + 1 < data.size() ? data[i + 1] : 0;
            uint32_t octet_c = i + 2 < data.size() ? data[i + 2] : 0;

            uint32_t triple = (octet_a << 16) + (octet_b << 8) + octet_c;

            encoded.push_back(encoding_table[(triple >> 18) & 0x3F]);
            encoded.push_back(encoding_table[(triple >> 12) & 0x3F]);
            encoded.push_back(encoding_table[(triple >> 6) & 0x3F]);
            encoded.push_back(encoding_table[triple & 0x3F]);
        }

        int mod_table[] = {0, 2, 1};
        int padding = mod_table[data.size() % 3];
        for (int i = 0; i < padding; i++) {
            encoded[encoded.size() - 1 - i] = '=';
        }

        return encoded;
    }

    std::vector<unsigned char> FileIO::readBinary(const std::string& filepath) {
        std::ifstream file(filepath, std::ios::binary | std::ios::ate);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file: " + filepath);
        }

        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        std::vector<unsigned char> buffer(size);
        if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
            throw std::runtime_error("Error reading file: " + filepath);
        }

        return buffer;
    }

    void FileIO::writeBinary(const std::string& filepath, const std::vector<unsigned char>& data) {
        std::ofstream file(filepath, std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file for writing: " + filepath);
        }

        file.write(reinterpret_cast<const char*>(data.data()), data.size());
        if (file.fail()) {
            throw std::runtime_error("Failed to write file: " + filepath);
        }
    }

    std::vector<std::future<Image>> ImageProcessor::processImagesAsync(const std::vector<std::string>& imagePaths, bool debug) {
        std::vector<std::future<Image>> futures;
        for (const auto& path : imagePaths) {
            futures.push_back(std::async(std::launch::async, [path, debug]() {
                try {
                    auto imageData = FileIO::readBinary(path);
                    std::string base64 = Base64::encode(imageData);
                    return Image(path, base64);
                } catch (const std::exception& e) {
                    errorLog(debug, "Error processing image: " + path + " - " + e.what());
                    return Image(path, "");
                }
            }));
        }
        return futures;
    }
}

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
        imageTags += "<img src=\"data:image/png;base64," + image.getBase64() + "\" alt=\"" + image.getPath() + "\">";
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

bool isFileReadable(const std::string& filepath) {
    std::ifstream file(filepath);
    return file.good();
}

// Melhorar validateImagePath
// Remover a versão global da função validateImagePath
// bool validateImagePath(const std::string& filepath, bool debug) { ... }

// Melhorar SPICEBuilder::addImage
SPICEBuilder& SPICEBuilder::addImage(const std::string& imagePath) {
    return addImagesAsync({imagePath});
}

SPICEBuilder& SPICEBuilder::addImagesAsync(const std::vector<std::string>& imagePaths) {
    tsimg::utils::debugLog(debug, "Adding images asynchronously");
    
    auto futures = tsimg::utils::ImageProcessor::processImagesAsync(imagePaths, debug);
    
    for (auto& future : futures) {
        try {
            Image img = future.get();
            if (!img.getBase64().empty()) {
                imageLists["SPICE_IMAGES"].addImage(img);
                tsimg::utils::debugLog(debug, "Image added successfully: " + img.getPath());
            }
        } catch (const std::exception& e) {
            tsimg::utils::errorLog(debug, "Failed to add image: " + std::string(e.what()));
        }
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

bool SPICEBuilder::hasAdditionalImages() const {
    auto it = imageLists.find("SPICE_IMAGES_1");
    return it != imageLists.end() && !it->second.getImages().empty();
}

TemplateWriter::TemplateWriter(const std::string& templatePath, bool debug) : templatePath(templatePath), debug(debug) {
    templateContent = tsimg::utils::FileHandler::readFile(templatePath, debug);
}

// Melhorar TemplateWriter::writeToFile
void TemplateWriter::writeToFile(const std::string& outputFile, 
                                 const std::vector<SpiceContent>& contents, 
                                 const std::map<std::string, ImageList>& imageLists, 
                                 const std::vector<std::string>& labels, 
                                 const std::string& authorImageBase64) {
    tsimg::utils::debugLog(debug, "Starting writeToFile process for: " + outputFile);

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

        // Procura pelos conteúdos de ajuda
        for (const auto& content : contents) {
            if (content.getTag() == "SPICE_HELP_TEXT") {
                helpText = content.getVariableContent();
            } else if (content.getTag() == "SPICE_HELP_CONTENT") {
                helpBadgeUrl = content.getVariableContent();
            } else if (content.getTag() == "SPICE_HELP_LINK") {
                helpLink = content.getVariableContent();
            }
        }

        // Cria a seção de ajuda apenas se todas as informações estiverem presentes
        std::string helpSection = tsimg::utils::HTMLBuilder::createHelpSection(
            helpText,
            helpBadgeUrl,
            helpLink
        );

        // Se não houver seção de ajuda, remove a tag completamente
        if (helpSection.empty()) {
            // Remove a tag e qualquer div container que a contenha
            size_t startPos = outputContent.find("<div class=\"help-section\">");
            if (startPos != std::string::npos) {
                size_t endPos = outputContent.find("</div>", startPos);
                if (endPos != std::string::npos) {
                    endPos += 6; // comprimento de "</div>"
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
        tsimg::utils::errorLog(debug, "Error in writeToFile: " + std::string(e.what()));
        throw; // Re-throw para permitir tratamento em nível superior
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

// Melhorar replaceTag com validação mais robusta
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
        }
    }

    return htmlContent;
}
