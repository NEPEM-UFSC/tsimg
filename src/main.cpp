#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <memory>
#include <nlohmann/json.hpp>
#include "tsimg_spice.h" 
#include "tsimg_gif.h"
#include "build_info.h"

using tsimg::utils::debugLog;
using tsimg::utils::errorLog;

// Interface para geradores de formato
class IFormatGenerator {
public:
    virtual ~IFormatGenerator() = default;
    virtual bool generate(const std::string& outputFilename, 
                          const std::vector<std::string>& imagePaths,
                          const std::vector<std::string>& labels,
                          const std::string& title,
                          const nlohmann::json& config) = 0;
};

// Implementação do gerador SPICE
class SPICEGenerator : public IFormatGenerator {
private:
    bool debug;

public:
    SPICEGenerator(bool debug) : debug(debug) {}

    bool generate(const std::string& outputFilename, 
                  const std::vector<std::string>& imagePaths,
                  const std::vector<std::string>& labels,
                  const std::string& title,
                  const nlohmann::json& config) override {
        try {
            // Criar builder e configurar
            SPICEBuilder builder(title, debug);
            builder.addTitle(title);
            builder.addImagesAsync(imagePaths);
            
            // Adicionar labels
            if (config.contains("createLabelsFromImages") && config["createLabelsFromImages"].get<bool>()) {
                builder.generateLabelsFromImages();
            }
            builder.addLabels(labels);
            
            // Configurar help se disponível
            extern std::string help_text, help_link, help_badge_url; // Use extern for global vars
            if (!help_text.empty() && !help_link.empty() && !help_badge_url.empty()) {
                builder.setHelp(help_text, help_link, help_badge_url);
            }
            
            // Configurar imagem do autor se disponível
            if (config.contains("author_image")) {
                builder.setAuthorImage(config["author_image"]);
            }
            
            // Configurar template se disponível
            if (config.contains("template")) {
                builder.setTemplate(config["template"]);
            }
              // Adicionar conteúdo principal se disponível
            if (config.contains("main_text")) {
                builder.addContent("SPICE_TEXT", config["main_text"]);
            }
            
            // Garantir que as imagens da lista principal são adicionadas a SPICE_IMAGES
            if (config.contains("images") && config["images"].is_array()) {
                for (const auto& img : config["images"]) {
                    builder.addImageToList("SPICE_IMAGES", img);
                }
            }
              
            // Processar imagens extras em listas separadas
            if (config.contains("extraImageLists") && config["extraImageLists"].is_array()) {
                int index = 1;
                for (const auto& imageList : config["extraImageLists"]) {
                    if (imageList.is_array()) {
                        std::string tag = "SPICE_IMAGES_" + std::to_string(index++);
                        for (const auto& img : imageList) {
                            builder.addImageToList(tag, img);
                        }
                    }
                }
            }
            
            // Processar também diretamente as listas images_1, images_2, etc.
            for (int i = 1; ; i++) {
                std::string key = "images_" + std::to_string(i);
                if (!config.contains(key) || !config[key].is_array())
                    break;
                
                std::string tag = "SPICE_IMAGES_" + std::to_string(i);
                for (const auto& img : config[key]) {
                    builder.addImageToList(tag, img);
                }
            }
            
            // Escrever arquivo final
            TemplateWriter writer(builder.getTemplatePath(), debug);
            writer.writeToFile(outputFilename, 
                              builder.getContents(), 
                              builder.getImageLists(), 
                              builder.getLabels(), 
                              builder.getAuthorImageBase64());
            
            tsimg::utils::debugLog(true, "SPICE file generated successfully: " + outputFilename);
            return true;
        } catch (const std::exception& e) {
            tsimg::utils::errorLog(true, "Failed to create SPICE file: " + std::string(e.what()));
            return false;
        }
    }
};

// Implementação do gerador GIF
class GIFGenerator : public IFormatGenerator {
private:
    bool debug;

public:
    GIFGenerator(bool debug) : debug(debug) {}

    bool generate(const std::string& outputFilename, 
                  const std::vector<std::string>& imagePaths,
                  const std::vector<std::string>& labels, // Unused in GIFGenerator
                  const std::string& title, // Unused in GIFGenerator
                  const nlohmann::json& config) override {
        try {
            // Configurar parâmetros do GIF
            int delay = 100; // padrão
            if (config.contains("gif_delay")) {
                delay = config["gif_delay"].get<int>();
            }
            
            bool loop = true; // padrão
            if (config.contains("gif_loop")) {
                loop = config["gif_loop"].get<bool>();
            }
            
            int quality = 75; // padrão. Note: quality is not a standard param for the gif.h library used.
                              // This parameter is not used in the createGif function call.
            if (config.contains("gif_quality")) {
                quality = config["gif_quality"].get<int>();
            }
            
            // Chamar função de criação de GIF com parâmetros configuráveis
            // The createGif function signature in tsimg_gif.h/cpp is:
            // bool createGif(const std::string& output_filename, const std::vector<std::string>& image_paths, bool debug, int delay = 100, bool loop = true);
            // It does not take a quality parameter. // This comment might be outdated given the linker error.
            if (createGif(outputFilename, imagePaths, debug, delay, loop, quality)) {
                tsimg::utils::debugLog(true, "GIF file generated successfully: " + outputFilename);
                return true;
            } else {
                return false;
            }
        } catch (const std::exception& e) {
            tsimg::utils::errorLog(true, "Error creating GIF: " + std::string(e.what()));
            return false;
        }
    }
};

// Constantes globais
const std::string DEFAULT_TITLE = "TSIMG Presentation";
const std::string APP_NAME = "Temporal Series Interactive Imager";
const std::string VERSION = "1.1.0";

// Definição de namespace tsimg para funções utilitárias
namespace tsimg::utils {

    // Função para dividir uma string em tokens
    std::vector<std::string> split(const std::string& str, char delimiter) {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream token_stream(str);
        while (std::getline(token_stream, token, delimiter)) {
            token.erase(0, token.find_first_not_of(' '));
            token.erase(token.find_last_not_of(' ') + 1);
            tokens.push_back(token);
        }
        return tokens;
    }

    // Função para concatenar strings
    std::string concatenateStrings(const std::vector<std::string>& vec) {
        std::ostringstream oss;
        for (const auto& str : vec) {
            oss << str;
        }
        return oss.str();
    }

    // Função para ler um arquivo para string com tratamento de erro aprimorado
    std::string readFileToString(const std::string& filepath, bool debug) {
        if (debug) std::cout << "Reading file: " << filepath << std::endl;
        
        if (!std::filesystem::exists(filepath)) {
            throw std::runtime_error("File does not exist: " + filepath);
        }

        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Error while trying to open file: " + filepath);
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        
        if (debug) std::cout << "File read successfully." << std::endl;
        return buffer.str();
    }

    // Função para ler um arquivo JSON com tratamento de erro aprimorado
    nlohmann::json read_json_file(const std::string& filename, bool debug) {
        if (debug) {
            std::cout << "Reading JSON config file: " << filename << std::endl;
        }
        
        if (!std::filesystem::exists(filename)) {
            throw std::runtime_error("JSON file does not exist: " + filename);
        }
        
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open JSON file: " + filename);
        }

        try {
            nlohmann::json json_data;
            file >> json_data;

            if (debug) {
                std::cout << "JSON file read successfully: " << filename << std::endl;
            }
            return json_data;
        } catch (const nlohmann::json::exception& e) {
            throw std::runtime_error("Error parsing JSON file: " + std::string(e.what()));
        }
    }

    // Classe para gerenciar configurações globais e ambiente
    class Environment {
    public:
        static void setupEnvironment() {
            // Verificar pastas críticas e criar se não existirem
            std::filesystem::path templatesPath = std::filesystem::current_path() / "templates";
            if (!std::filesystem::exists(templatesPath)) {
                std::filesystem::create_directory(templatesPath);
            }

            std::filesystem::path outputPath = std::filesystem::current_path() / "output";
            if (!std::filesystem::exists(outputPath)) {
                std::filesystem::create_directory(outputPath);
            }
        }
    };

    // Factory para criação de geradores de formato
    class FormatGeneratorFactory {
    public:
        static std::unique_ptr<::IFormatGenerator> createGenerator(const std::string& format, bool debug) {
            if (format == "spice") {
                return std::make_unique<::SPICEGenerator>(debug);
            } else if (format == "gif") {
                return std::make_unique<::GIFGenerator>(debug);
            } else {
                throw std::runtime_error("Unsupported format: " + format);
            }
        }
    };

    // Logger para centralizar logs
    class Logger {
    public:
        static void debug(bool debugEnabled, const std::string& message) {
            if (debugEnabled) {
                std::cout << "[DEBUG] " << message << std::endl;
            }
        }

        static void info(const std::string& message) {
            std::cout << "[INFO] " << message << std::endl;
        }

        static void error(const std::string& message) {
            std::cerr << "[ERROR] " << message << std::endl;
        }
    };

    // Cache para evitar reprocessamento de imagens
    // MOVIDA PARA TSIMG_SPICE.H
    // class ImageCache {
    // private:
    //     static std::map<std::string, std::shared_ptr<Image>> cache;
    // public:
    //     static std::shared_ptr<Image> getImage(const std::string& path, bool debug);
    //     static void clearCache();
    // };

    // Inicialização do mapa de cache
    // MOVIDA PARA TSIMG_SPICE.CPP OU ONDE ImageCache É DEFINIDA
    // std::map<std::string, std::shared_ptr<Image>> ImageCache::cache;

} // namespace tsimg::utils

// Variáveis globais para ajuda
std::string help_text;
std::string help_badge_url;
std::string help_link;

bool app_info = false;

// Gerenciador de pipeline de processamento
class TSIMGPipeline {
private:
    bool debug;
    std::string format;
    std::string outputFilename;
    std::vector<std::string> imagePaths;
    std::vector<std::string> labels;
    std::string title;
    nlohmann::json config;

public:
    TSIMGPipeline(bool debug) : debug(debug) {
        title = DEFAULT_TITLE;
        format = "spice"; // formato padrão
    }

    TSIMGPipeline& setFormat(const std::string& fmt) {
        format = fmt;
        return *this;
    }

    TSIMGPipeline& setOutputFilename(const std::string& filename) {
        outputFilename = filename;
        return *this;
    }

    TSIMGPipeline& setImagePaths(const std::vector<std::string>& paths) {
        imagePaths = paths;
        return *this;
    }

    TSIMGPipeline& setLabels(const std::vector<std::string>& lbls) {
        labels = lbls;
        return *this;
    }

    TSIMGPipeline& setTitle(const std::string& ttl) {
        title = ttl.empty() ? DEFAULT_TITLE : ttl;
        return *this;
    }

    TSIMGPipeline& setConfig(const nlohmann::json& cfg) {
        config = cfg;
        return *this;
    }

    TSIMGPipeline& loadFromJsonConfig(const std::string& jsonPath) {
        try {
            config = tsimg::utils::read_json_file(jsonPath, debug);
            
            // Aplicar configurações do JSON
            if (config.contains("export_format")) {
                format = config["export_format"];
            }
            
            if (config.contains("output_filename")) {
                outputFilename = config["output_filename"];
            }
            
            if (config.contains("title")) {
                title = config["title"];
            }
            
            if (config.contains("labels") && config["labels"].is_array()) {
                labels.clear();
                for (const auto& label : config["labels"]) {
                    labels.push_back(label);
                }
            }
              if (config.contains("images") && config["images"].is_array()) {
                imagePaths.clear();
                for (const auto& img : config["images"]) {
                    imagePaths.push_back(img);
                }
            }
            
            // Processar listas de imagens adicionais (images_1, images_2, etc.)
            for (int i = 1; ; i++) {
                std::string key = "images_" + std::to_string(i);
                if (!config.contains(key) || !config[key].is_array())
                    break;
                
                // Criar um vetor para esta lista de imagens
                std::vector<std::string> extraImageList;
                for (const auto& img : config[key]) {
                    extraImageList.push_back(img);
                }
                
                // Se não existir a lista extraImageLists no config, criá-la
                if (!config.contains("extraImageLists")) {
                    config["extraImageLists"] = nlohmann::json::array();
                }
                
                // Adicionar esta lista ao config
                config["extraImageLists"].push_back(extraImageList);
            }
            
            // Configurações de ajuda
            if (config.contains("help_text")) {
                help_text = config["help_text"];
            }
            
            if (config.contains("help_link")) {
                help_link = config["help_link"];
            }
            
            if (config.contains("help_badge_url")) {
                help_badge_url = config["help_badge_url"];
            }
        } catch (const std::exception& e) {
            tsimg::utils::errorLog(true, "Error loading JSON config: " + std::string(e.what()));
            throw;
        }
        return *this;
    }

    bool validate() {
        // Validar formato
        if (format != "spice" && format != "gif") {
            tsimg::utils::errorLog(true, "Unsupported format: " + format);
            return false;
        }
        
        // Validar nome do arquivo de saída
        if (outputFilename.empty()) {
            tsimg::utils::errorLog(true, "Output filename cannot be empty");
            return false;
        }
        
        // Validar caminhos de imagem
        if (imagePaths.empty()) {
            tsimg::utils::errorLog(true, "No image paths provided");
            return false;
        }
        
        for (const auto& img : imagePaths) {
            if (!tsimg::utils::ImageValidator::validateImagePath(img, debug)) {
                tsimg::utils::errorLog(true, "Invalid image file: " + img);
                return false;
            }
        }
        
        return true;
    }

    bool execute() {
        if (!validate()) {
            return false;
        }
        
        try {
            // Criar gerador de formato apropriado
            std::unique_ptr<IFormatGenerator> generator = tsimg::utils::FormatGeneratorFactory::createGenerator(format, debug); // Explicit type
            
            // Executar a geração
            return generator->generate(outputFilename, imagePaths, labels, title, config);
        } catch (const std::exception& e) {
            tsimg::utils::errorLog(true, "Error in pipeline execution: " + std::string(e.what()));
            return false;
        }
    }
};

// Função para exibir informações do aplicativo e ajuda
void display_info() {
    if (app_info) {
        std::cout << "===================================================\n" << std::endl;
        std::cout << " /$$$$$$$$  /$$$$$$  /$$$$$$ /$$      /$$  /$$$$$$ " << std::endl;
        std::cout << "|__  $$__/ /$$__  $$|_  $$_/| $$$    /$$$ /$$__  $$" << std::endl;
        std::cout << "   | $$   | $$  \\__/  | $$  | $$$$  /$$$$| $$  \\__/" << std::endl;
        std::cout << "   | $$   |  $$$$$$   | $$  | $$ $$/$$ $$| $$ /$$$$" << std::endl;
        std::cout << "   | $$    \\____  $$  | $$  | $$  $$$| $$| $$|_  $$" << std::endl;
        std::cout << "   | $$    /$$  \\ $$  | $$  | $$\\  $ | $$| $$  \\ $$" << std::endl;
        std::cout << "   | $$   |  $$$$$$/ /$$$$$$| $$ \\/  | $$|  $$$$$$/" << std::endl;
        std::cout << "   |__/    \\______/ |______/|__/     |__/ \\______/ " << std::endl;
        std::cout << "\n===================================================" << std::endl;
        std::cout << " \nTemporal Series Interactive Imager (TSIMG) v" << VERSION << "\n" << std::endl;
        std::cout << "===================================================" << std::endl;
        std::cout << "\n A tool for creating interactive images of time series." << std::endl;
        std::cout << "Supports export to SPICE and GIF formats." << std::endl;
        std::cout << "For more information, please visit: \nhttps://github.com/NEPEM-UFSC/tsimg" << std::endl;
        std::cout << "\n===================================================\n" << std::endl;
        #ifdef BUILD_INFO
            std::cout << BUILD_INFO << std::endl;
        #else
            std::cout << "Build information not available." << std::endl;
        #endif
    }
    std::cerr << "Usage: create_file -n <output_filename> -i <image1.jpg,image2.png,...> [-l <label1,label2,...>] [-f <format>] [-debug] [-config <config.json>]" << std::endl;
    std::cerr << "Options:" << std::endl;
    std::cerr << "  -n <output_filename>    Specify the output filename." << std::endl;
    std::cerr << "  -i <image_paths>        Comma-separated list of image paths." << std::endl;
    std::cerr << "  -l <labels>             Comma-separated list of labels (optional)." << std::endl;
    std::cerr << "  -f <format>             Output format: 'spice' or 'gif' (default: 'spice')." << std::endl;
    std::cerr << "  -debug                  Enable debug mode (optional)." << std::endl;
    std::cerr << "  -config <config.json>   Path to JSON config file (optional)." << std::endl;
    std::cerr << "  -labelbyname           Generate labels from image names (optional)." << std::endl;
    std::cerr << "  -authorimage <author_image_path>   Path to author image (optional)." << std::endl;
    std::cerr << "  -help_text <text>       Help text to display (optional)." << std::endl;
    std::cerr << "  -help_link <link>       Help link URL (optional)." << std::endl;
    std::cerr << "  -help_badge_url <url>   Help badge image URL (optional)." << std::endl;
    std::cerr << "  -template <template_path> Path to custom HTML template (optional)." << std::endl;
    std::cerr << "  -title <title>          Title for the presentation (optional)." << std::endl;
}

// Função para validar configuração JSON com validação mais robusta
bool validateJsonConfig(const nlohmann::json& config, bool debug) {
    // Verificar campos obrigatórios
    const std::vector<std::string> required = {"export_format", "output_filename"};
    for (const auto& field : required) {
        if (!config.contains(field)) {
            if (debug) std::cerr << "Error: Missing required field in JSON config: " << field << std::endl;
            return false;
        }
    }
    
    // Validar formato de exportação
    std::string format = config["export_format"];
    if (format != "spice" && format != "gif") {
        if (debug) std::cerr << "Error: Invalid export format in config: " << format << std::endl;
        return false;
    }
    
    // Validar imagens se presentes
    for (int i = 0; ; ++i) {
        std::string key = "images" + (i == 0 ? "" : "_" + std::to_string(i));
        if (!config.contains(key)) break;
        
        if (!config[key].is_array()) {
            if (debug) std::cerr << "Error: " << key << " must be an array" << std::endl;
            return false;
        }
        
        for (const auto& img : config[key]) {
            if (!img.is_string()) {
                if (debug) std::cerr << "Error: Image path must be a string" << std::endl;
                return false;
            }
            // Usar a função do namespace
            if (!tsimg::utils::ImageValidator::validateImagePath(img, debug)) {
                return false;
            }
        }
    }
    
    return true;
}

// Função para modo interativo aprimorado
bool runInteractiveMode() {
    app_info = true;
    display_info();

    tsimg::utils::debugLog(true, "Por favor, insira os parâmetros necessários.");

    std::string output_filename;
    std::string images_input;
    std::string labels_input;
    std::string format = "spice";
    bool debug = false;
    std::string title;

    // Prompt para título
    std::cout << "Título da apresentação (pressione Enter para usar o padrão): ";
    std::getline(std::cin, title);
    if (title.empty()) {
        title = DEFAULT_TITLE;
    }

    std::cout << "Nome do arquivo de saída: ";
    std::getline(std::cin, output_filename);

    std::cout << "Caminhos das imagens (separados por vírgula): ";
    std::getline(std::cin, images_input);
    std::vector<std::string> image_paths = tsimg::utils::split(images_input, ',');

    std::cout << "Labels (opcional, separados por vírgula): ";
    std::getline(std::cin, labels_input);
    std::vector<std::string> labels = tsimg::utils::split(labels_input, ',');

    std::cout << "Formato de exportação ('spice' ou 'gif', padrão: 'spice'): ";
    std::string format_input;
    std::getline(std::cin, format_input);
    if (!format_input.empty()) {
        format = format_input;
    }

    std::cout << "Ativar modo debug? (s/n): ";
    std::string debug_input;
    std::getline(std::cin, debug_input);
    if (debug_input == "s" || debug_input == "S") {
        debug = true;
    }

    // Configurações avançadas opcionais
    std::cout << "Deseja configurar opções avançadas? (s/n): ";
    std::string advanced_input;
    std::getline(std::cin, advanced_input);
    
    nlohmann::json config;
    if (advanced_input == "s" || advanced_input == "S") {
        std::cout << "Caminho da imagem do autor (opcional): ";
        std::string author_image;
        std::getline(std::cin, author_image);
        if (!author_image.empty()) {
            config["author_image"] = author_image;
        }
        
        std::cout << "Texto de ajuda (opcional): ";
        std::getline(std::cin, help_text);
        
        if (!help_text.empty()) {
            std::cout << "Link de ajuda (opcional): ";
            std::getline(std::cin, help_link);
            
            std::cout << "URL do ícone da ajuda (opcional): ";
            std::getline(std::cin, help_badge_url);
        }
        
        std::cout << "Caminho do template personalizado (opcional): ";
        std::string template_path;
        std::getline(std::cin, template_path);
        if (!template_path.empty()) {
            config["template"] = template_path;
        }
    }

    // Processamento usando o novo pipeline
    try {
        TSIMGPipeline pipeline(debug);
        pipeline.setFormat(format)
                .setOutputFilename(output_filename)
                .setImagePaths(image_paths)
                .setLabels(labels)
                .setTitle(title)
                .setConfig(config);
        
        return pipeline.execute();
    } catch (const std::exception& e) {
        tsimg::utils::errorLog(true, "Erro durante o processamento: " + std::string(e.what()));
        return false;
    }
}

// Função main refatorada
int main(int argc, char* argv[]) {
    // Configurar ambiente
    tsimg::utils::Environment::setupEnvironment();
    
    // Verificar modo interativo
    if (argc == 1) {
        return runInteractiveMode() ? 0 : 1;
    }
    
    // Processamento de argumentos de linha de comando
    std::string output_filename;
    std::vector<std::string> image_paths;
    std::vector<std::string> labels;
    bool debug = false;
    bool createLabelsFromImages = false;
    std::string format = "spice";
    std::string json_config_file;
    std::string author_image_path;
    std::string template_path;
    std::string title = DEFAULT_TITLE;

    std::vector<std::vector<std::string>> imagePathsExtras;

    // Processar argumentos de linha de comando
    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "-info") == 0) {
            app_info = true;
            display_info();
            app_info = false;
            return 0;
        } else if (std::strcmp(argv[i], "-n") == 0 && i + 1 < argc) {
            output_filename = argv[++i];
        } else if (std::strcmp(argv[i], "-i") == 0 && i + 1 < argc) {
            image_paths = tsimg::utils::split(argv[++i], ',');
        } else if (std::strcmp(argv[i], "-l") == 0 && i + 1 < argc) {
            labels = tsimg::utils::split(argv[++i], ',');
        } else if (std::strcmp(argv[i], "-debug") == 0) {
            debug = true;
        } else if (std::strcmp(argv[i], "-f") == 0 && i + 1 < argc) {
            format = argv[++i];
        } else if (std::strcmp(argv[i], "-config") == 0 && i + 1 < argc) {
            json_config_file = argv[++i];
        } else if (std::strcmp(argv[i], "-labelbyname") == 0) {
            createLabelsFromImages = true;
        } else if (std::strcmp(argv[i], "-authorimage") == 0 && i + 1 < argc) {
            author_image_path = argv[++i];
        } else if (std::strcmp(argv[i], "-help_text") == 0 && i + 1 < argc) {
            help_text = argv[++i];
        } else if (std::strcmp(argv[i], "-help_link") == 0 && i + 1 < argc) {
            help_link = argv[++i];
        } else if (std::strcmp(argv[i], "-help_badge_url") == 0 && i + 1 < argc) {
            help_badge_url = argv[++i];
        } else if (std::strcmp(argv[i], "-title") == 0 && i + 1 < argc) {
            title = argv[++i];
        } else if (std::strncmp(argv[i], "-2", 2) == 0 && i + 1 < argc) {
            imagePathsExtras.push_back(tsimg::utils::split(argv[++i], ','));
        } else if (std::strncmp(argv[i], "-3", 2) == 0 && i + 1 < argc) {
            imagePathsExtras.push_back(tsimg::utils::split(argv[++i], ','));
        } else if (std::strcmp(argv[i], "-template") == 0 && i + 1 < argc) {
            template_path = argv[++i];
        }
    }

    // Criar pipeline e configurar
    TSIMGPipeline pipeline(debug);
    nlohmann::json config;
    
    // Adicionar configurações ao objeto JSON
    if (!author_image_path.empty()) {
        config["author_image"] = author_image_path;
    }
    
    if (!template_path.empty()) {
        config["template"] = template_path;
    }
    
    config["createLabelsFromImages"] = createLabelsFromImages;
    
    // Adicionar listas extras de imagens
    if (!imagePathsExtras.empty()) {
        config["extraImageLists"] = imagePathsExtras;
    }

    try {
        // Se tiver arquivo de configuração JSON, usar ele
        if (!json_config_file.empty()) {
            pipeline.loadFromJsonConfig(json_config_file);
        } else {
            // Configurar pipeline com argumentos de linha de comando
            pipeline.setFormat(format)
                    .setOutputFilename(output_filename)
                    .setImagePaths(image_paths)
                    .setLabels(labels)
                    .setTitle(title)
                    .setConfig(config);
        }
        
        // Executar o pipeline
        if (pipeline.execute()) {
            return 0;
        } else {
            return 1;
        }
    } catch (const std::exception& e) {
        tsimg::utils::errorLog(true, e.what());
        display_info();
        return 1;
    }
}