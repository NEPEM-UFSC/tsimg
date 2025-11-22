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
#include "core/TSIMGPipeline.h"

using tsimg::utils::debugLog;
using tsimg::utils::errorLog;







// Constantes globais
const std::string DEFAULT_TITLE = "TSIMG Presentation";
const std::string APP_NAME = "Temporal Series Interactive Imager";

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
        std::cout << " \nTemporal Series Interactive Imager (TSIMG) v" << tsimg::getVersion() << "\n" << std::endl;
        std::cout << "===================================================" << std::endl;
        std::cout << "\n A tool for creating interactive images of time series." << std::endl;
        std::cout << "Supports export to SPICE and GIF formats." << std::endl;
        std::cout << "For more information, please visit: \nhttps://github.com/NEPEM-UFSC/tsimg" << std::endl;
        std::cout << "\n===================================================\n" << std::endl;
        
        std::cout << tsimg::getBuildInfo() << std::endl;
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
        tsimg::core::TSIMGPipeline pipeline(debug);
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
    tsimg::core::TSIMGPipeline pipeline(debug);
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