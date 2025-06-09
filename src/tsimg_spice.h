#pragma once

#include <string>
#include <vector>
#include <map>
#include <future>
#include <thread>
#include <memory>
#include <chrono>
#include <functional>
#include <optional>

// Declaração antecipada para Image
class Image;

// Interface para processamento de imagens
class IImageProcessor {
public:
    virtual ~IImageProcessor() = default;
    virtual void processImage(const std::string& path) = 0;
    virtual bool supportsFormat(const std::string& extension) = 0;
    virtual std::vector<std::string> getSupportedFormats() const = 0;
};

// Classe para representar uma imagem
class Image {
public:
    Image(const std::string& path, const std::string& base64Data);
    Image(const std::string& path); // Versão simples para carregamento sob demanda
    ~Image();

    // Getters
    const std::string& getPath() const;
    int getWidth() const;
    int getHeight() const;
    
    // Funções para manipulação de imagem
    bool resize(int width, int height);
    bool crop(int x, int y, int width, int height);
    bool rotate(float angle);
    bool applyFilter(const std::string& filterName);
    
    // Funções para conversão
    bool convertToFormat(const std::string& format);
    
    // Funções para validação
    bool isValid() const;
    
    // Funções para renderização
    std::string toHtml() const;
    std::string toBase64() const;
    void saveToFile(const std::string& outputPath) const;

private:
    std::string path;
    std::string base64Data;
    int width;
    int height;
    bool isLoaded;
    
    // Função interna para carregar a imagem
    bool loadImage();
    bool encodeToBase64();
};

// Classe para representar uma lista de imagens
class ImageList {
public:
    ImageList();
    ~ImageList();

    void addImage(std::unique_ptr<Image> image);
    void addImage(const std::string& path);
    void addImages(const std::vector<std::string>& paths);
    void addImagesAsync(const std::vector<std::string>& paths);
    std::vector<std::unique_ptr<Image>>& getImages();
    void clear();
    size_t size() const;
    bool empty() const;
    std::string generateImageTags() const;

private:
    std::vector<std::unique_ptr<Image>> images;
};

// Classe para representar conteúdo SPICE
class SpiceContent {
public:
    SpiceContent(const std::string& tag, const std::string& content);
    const std::string& getTag() const;
    const std::string& getVariableContent() const;

private:
    std::string tag;
    std::string variableContent;
};

// Classe Builder para criar documentos SPICE
class SPICEBuilder {
public:
    SPICEBuilder();
    SPICEBuilder(const std::string& title, bool debug);
    
    // Métodos para adicionar conteúdo
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
    
    // Métodos para acessar informações
    const std::vector<SpiceContent>& getContents() const;
    const std::map<std::string, std::unique_ptr<ImageList>>& getImageLists() const;
    const std::vector<std::string>& getLabels() const;
    const std::string& getAuthorImageBase64() const;
    const std::string& getTitle() const;
    const std::string& getTemplatePath() const;
    
    // Métodos para gerar tags
    std::string getImageTags() const;
    std::string generateImageTags() const;
    std::string generateLabelTags() const;
    
    // Método para depuração
    void debugPrint() const;
    
    // Método para verificar estado
    bool hasAdditionalImages() const;

private:
    std::string title;
    bool debug;
    std::vector<SpiceContent> contents;
    std::map<std::string, std::unique_ptr<ImageList>> imageLists;
    std::vector<std::string> labels;
    std::string authorImageBase64;
    std::string templatePath;
    
    // Métodos privados de utilidade
    std::string extractLabelFromPath(const std::string& path) const;
    void processImagesInBackground(const std::vector<std::string>& paths);
};

// Classe para representar um documento SPICE
class SPICE {
public:
    SPICE(const std::string& title, bool debug);
    void generateSPICEFileFromTemplate(const std::string& templateFile, const std::string& outputFile, bool debug);
    std::string generateLabelTags();
    void debugPrint() const;
    
    // Adicionar novos métodos
    void addContent(const SpiceContent& content);
    void addImageList(const std::string& tag, std::unique_ptr<ImageList> imageList);
    void addLabels(const std::vector<std::string>& newLabels);
    
    // Métodos para configurações
    void setTitle(const std::string& newTitle);
    void setAuthorImage(const std::string& imagePath);
    const std::string& getTitle() const;

private:
    std::string title;
    bool debug;
    std::vector<SpiceContent> contents;
    std::map<std::string, std::unique_ptr<ImageList>> imageLists;
    std::vector<std::string> labels;
    std::string authorImageBase64;
};

// Classe para escrever templates
class TemplateWriter {
public:
    TemplateWriter(const std::string& templatePath, bool debug);
    
    // Métodos estáticos
    static std::string getDefaultTemplatePath();
    static std::string resolveTemplatePath(const std::string& templateName);
    
    // Métodos para escrita
    void writeToFile(const std::string& outputFile, 
                     const std::vector<SpiceContent>& contents, 
                     const std::map<std::string, std::unique_ptr<ImageList>>& imageLists, 
                     const std::vector<std::string>& labels, 
                     const std::string& authorImageBase64);
    
    // Métodos para builder
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

// Função para encoding de imagens
std::string encodeImageToBase64(const std::string& imagePath, bool debug);

// Função para verificar se um arquivo é legível
bool isFileReadable(const std::string& filepath);

// Namespace para utilitários do TSIMG
namespace tsimg::utils {
    // Funções de logging
    void debugLog(bool debug, const std::string& message);
    void errorLog(bool debug, const std::string& message);
    
    // Handler de arquivos
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

    // Construtor de HTML
    class HTMLBuilder {
    public:
        static std::string createHelpSection(
            const std::string& helpText, 
            const std::string& helpContent, 
            const std::string& helpLink
        );
        static std::string createLabelTags(const std::vector<std::string>& labels);
        
        // Novos métodos
        static std::string createImageGallery(const std::vector<std::string>& imagePaths);
        static std::string createSlider(const std::vector<std::string>& imagePaths, const std::vector<std::string>& labels);
        static std::string createHeader(const std::string& title, const std::string& subtitle = "");
        static std::string createFooter(const std::string& copyright = "", const std::string& links = "");
    };

    // Validador de imagens
    class ImageValidator {
    public:
        static bool validateImagePath(const std::string& filepath, bool debug = false);
        
        // Novos métodos
        static bool validateImageDimensions(const std::string& filepath, int minWidth, int minHeight);
        static bool validateImageFormat(const std::string& filepath, const std::vector<std::string>& allowedFormats);
        static std::vector<std::string> getSupportedFormats();
    };

    // Utilitário de Base64
    class Base64 {
    public:
        static std::string encode(const std::vector<unsigned char>& data);
        
        // Novos métodos
        static std::vector<unsigned char> decode(const std::string& encodedData);
        static std::string encodeFromFile(const std::string& filepath);
    };

    // IO de arquivos binários
    class FileIO {
    public:
        static std::vector<unsigned char> readBinary(const std::string& filepath);
        static void writeBinary(const std::string& filepath, const std::vector<unsigned char>& data);
        
        // Novos métodos
        static bool copyFile(const std::string& source, const std::string& destination);
        static bool moveFile(const std::string& source, const std::string& destination);
        static bool deleteFile(const std::string& filepath);
        static bool createDirectory(const std::string& dirPath);
    };

    // Processador de imagens
    class ImageProcessor {
    public:
        static std::vector<std::future<std::unique_ptr<Image>>> processImagesAsync(
            const std::vector<std::string>& imagePaths, bool debug);
        
        // Novos métodos
        static std::unique_ptr<Image> processImage(const std::string& imagePath, bool debug);
        static std::unique_ptr<Image> resizeImage(const std::string& imagePath, int width, int height, bool debug);
        static std::unique_ptr<Image> cropImage(const std::string& imagePath, int x, int y, int width, int height, bool debug);
        static std::unique_ptr<Image> rotateImage(const std::string& imagePath, float angle, bool debug);
    };

    // Cache de imagens
    class ImageCache {
    public:
        static void addToCache(const std::string& key, std::shared_ptr<Image> image);
        static std::shared_ptr<Image> getFromCache(const std::string& key);
        static bool isInCache(const std::string& key);
        static void clearCache();
        static size_t getCacheSize();
        
    private:
        static std::map<std::string, std::shared_ptr<Image>> cache;
        static std::chrono::system_clock::time_point lastCacheClear;
    };

    // Monitorador de progresso
    class ProgressMonitor {
    public:
        ProgressMonitor(int total, const std::string& operation = "Processing");
        void update(int current);
        void complete();
        
    private:
        int total;
        int current;
        std::string operation;
        std::chrono::system_clock::time_point startTime;
    };

    // Criador de Template
    class TemplateFactory {
    public:
        static std::string createTemplate(const std::string& type, 
                                         const std::map<std::string, std::string>& options);
        static std::vector<std::string> getAvailableTemplates();
        static bool validateTemplate(const std::string& templateContent);
    };

    // Funções utilitárias
    std::string getTemplateContent(const std::string& templatePath, bool debug);
    std::string extractFileName(const std::string& path);
    std::string getFileExtension(const std::string& path);
    std::string sanitizeFileName(const std::string& filename);
    std::string getCurrentDateTime(const std::string& format = "%Y-%m-%d %H:%M:%S");
}

// Factory para processadores de imagem
class ImageProcessorFactory {
public:
    static std::unique_ptr<IImageProcessor> createProcessor(const std::string& format);
    static std::vector<std::string> getSupportedFormats();
    
private:
    static std::map<std::string, std::function<std::unique_ptr<IImageProcessor>()>> processors;
};

// Pipeline de processamento de imagens
class ImageProcessingPipeline {
public:
    ImageProcessingPipeline();
    
    // Configuração do pipeline
    ImageProcessingPipeline& addStage(std::function<void(Image&)> processor);
    ImageProcessingPipeline& setInputPath(const std::string& path);
    ImageProcessingPipeline& setOutputPath(const std::string& path);
    
    // Execução
    bool execute();
    std::unique_ptr<Image> getResult();
    
private:
    std::vector<std::function<void(Image&)>> stages;
    std::string inputPath;
    std::string outputPath;
    std::unique_ptr<Image> result;
};

// Gerenciador de coleções de imagens
class ImageCollectionManager {
public:
    ImageCollectionManager();
    
    // Métodos de gerenciamento
    void addCollection(const std::string& name, std::unique_ptr<ImageList> collection);
    std::unique_ptr<ImageList> getCollection(const std::string& name);
    bool hasCollection(const std::string& name) const;
    void removeCollection(const std::string& name);
    
    // Métodos de processamento
    void processAllCollections(std::function<void(ImageList&)> processor);
    
private:
    std::map<std::string, std::unique_ptr<ImageList>> collections;
};

// Analisador de imagens
class ImageAnalyzer {
public:
    // Métodos de análise
    static std::map<std::string, double> analyzeHistogram(const Image& image);
    static double calculateSimilarity(const Image& image1, const Image& image2);
    static std::vector<double> extractFeatures(const Image& image);
    
    // Métodos de detecção
    static std::vector<std::pair<int, int>> detectEdges(const Image& image);
    static std::optional<std::pair<int, int>> detectMainObject(const Image& image);
};
