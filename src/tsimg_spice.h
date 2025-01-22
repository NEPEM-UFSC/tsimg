#pragma once

#include <string>
#include <vector>
#include <map>
#include <future>
#include <thread>
#include <memory>

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
    void addImage(std::unique_ptr<Image> image);
    std::vector<std::unique_ptr<Image>>& getImages();
    std::string generateImageTags() const;

private:
    std::vector<std::unique_ptr<Image>> images;
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
    SPICEBuilder();
    SPICEBuilder(const std::string& title, bool debug);
    SPICEBuilder& addImage(const std::string& imagePath);
    SPICEBuilder& addImageToList(const std::string& listTag, const std::string& imagePath);
    SPICEBuilder& addContent(const std::string& tag, const std::string& content);
    SPICEBuilder& addLabels(const std::vector<std::string>& labelList);
    SPICEBuilder& generateLabelsFromImages();
    SPICEBuilder& setAuthorImage(const std::string& imagePath);
    SPICEBuilder& setHelp(const std::string& helpText, const std::string& helpLink, const std::string& helpBadgeURL);
    SPICEBuilder& addTitle(const std::string& title);
    SPICEBuilder& addImagesAsync(const std::vector<std::string>& imagePaths);
    SPICEBuilder& setTemplate(const std::string& templatePath);
    const std::vector<SpiceContent>& getContents() const;
    const std::map<std::string, std::unique_ptr<ImageList>>& getImageLists() const;
    const std::vector<std::string>& getLabels() const;
    const std::string& getAuthorImageBase64() const;
    const std::string& getTitle() const;
    const std::string& getTemplatePath() const;
    std::string getImageTags() const;
    std::string generateImageTags() const;
    std::string generateLabelTags() const;
    void debugPrint() const;
    bool hasAdditionalImages() const;

private:
    std::string title;
    bool debug;
    std::vector<SpiceContent> contents;
    std::map<std::string, std::unique_ptr<ImageList>> imageLists;
    std::vector<std::string> labels;
    std::string authorImageBase64;
    std::string templatePath;
};

class SPICE {
public:
    SPICE(const std::string& title, bool debug);
    void generateSPICEFileFromTemplate(const std::string& templateFile, const std::string& outputFile, bool debug);
    std::string generateLabelTags();
    void debugPrint() const;

private:
    std::string title;
    bool debug;
    std::vector<SpiceContent> contents;
    std::map<std::string, std::unique_ptr<ImageList>> imageLists;
    std::vector<std::string> labels;
    std::string authorImageBase64;
};

class TemplateWriter {
public:
    TemplateWriter(const std::string& templatePath, bool debug);
    static std::string getDefaultTemplatePath();
    static std::string resolveTemplatePath(const std::string& templateName);
    void writeToFile(const std::string& outputFile, 
                     const std::vector<SpiceContent>& contents, 
                     const std::map<std::string, std::unique_ptr<ImageList>>& imageLists, 
                     const std::vector<std::string>& labels, 
                     const std::string& authorImageBase64);
    void build(const SPICEBuilder& builder, const std::string& outputFile);
    std::string buildHtmlStructure(const SPICEBuilder& builder);

private:
    static const std::string VERSION;
    std::string generateBuildInfo() const;
    std::string getCurrentDateTime() const;
    std::string formatBuildInfo(const std::string& buildInfo) const;
    std::string readFileToString(const std::string& filePath);
    std::string replaceTag(const std::string& source, const std::string& tag, const std::string& replacement);
    bool validateImageListAndLabels(const std::map<std::string, std::unique_ptr<ImageList>>& imageLists, const std::vector<std::string>& labels);
    std::string replaceAllTags(const std::string& source, const std::vector<SpiceContent>& contents);
    std::string replaceObjectPlaceholders(const std::string& source, const std::map<std::string, std::unique_ptr<ImageList>>& imageLists);

    std::string templatePath;
    std::string templateContent;
    bool debug;
};

std::string encodeImageToBase64(const std::string& imagePath, bool debug);

// Adicionar após as outras declarações
bool isFileReadable(const std::string& filepath);

// Funções de validação de imagem
namespace tsimg::utils {
    void debugLog(bool debug, const std::string& message);
    void errorLog(bool debug, const std::string& message);
    
    class FileHandler {
    public:
        static std::string readFile(const std::string& filepath, bool debug = false);
        static void writeFile(const std::string& filepath, const std::string& content, bool debug = false);
        static bool isValidImageFormat(const std::string& filepath);
        static bool isFileReadable(const std::string& filepath);
        
    private:
        static void validateFilePath(const std::string& filepath);
        static void createDirectoryIfNeeded(const std::string& filepath);
        static std::string readFileContent(std::ifstream& file, const std::string& filepath, bool debug);
        static void writeFileContent(std::ofstream& file, const std::string& content);
        static std::string getFileExtension(const std::string& filepath);
    };

    class HTMLBuilder {
    public:
        static std::string createHelpSection(
            const std::string& helpText, 
            const std::string& helpContent, 
            const std::string& helpLink
        );
        static std::string createLabelTags(const std::vector<std::string>& labels);
    };

    class ImageValidator {
    public:
        static bool validateImagePath(const std::string& filepath, bool debug = false);
    };

    class Base64 {
    public:
        static std::string encode(const std::vector<unsigned char>& data);
    };

    class FileIO {
    public:
        static std::vector<unsigned char> readBinary(const std::string& filepath);
        static void writeBinary(const std::string& filepath, const std::vector<unsigned char>& data);
    };

    class ImageProcessor {
    public:
        static std::vector<std::future<std::unique_ptr<Image>>> processImagesAsync(const std::vector<std::string>& imagePaths, bool debug);
    };

    std::string getTemplateContent(const std::string& templatePath, bool debug);
}
