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
#include "utils/ConfigManager.h"

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
// Removidas para evitar estado global
// std::string help_text;
// std::string help_badge_url;
// std::string help_link;
// bool app_info = false;

// Função para exibir informações do aplicativo e ajuda
void display_info(bool app_info) {
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
    display_info(true);

    tsimg::utils::debugLog(true, "Por favor, insira os parâmetros necessários.");

    tsimg::utils::ConfigManager configManager;
    nlohmann::json config;

    std::string input;
    
    // Prompt para título
    std::cout << "Título da apresentação (pressione Enter para usar o padrão): ";
    std::getline(std::cin, input);
    if (!input.empty()) {
        config["title"] = input;
    }

    std::cout << "Nome do arquivo de saída: ";
    std::getline(std::cin, input);
    config["output_filename"] = input;

    std::cout << "Caminhos das imagens (separados por vírgula): ";
    std::getline(std::cin, input);
    // Usar split do ConfigManager seria ideal, mas ele é privado ou helper.
    // Vamos usar o split local que ainda existe no namespace utils
    config["images"] = tsimg::utils::split(input, ',');

    std::cout << "Labels (opcional, separados por vírgula): ";
    std::getline(std::cin, input);
    config["labels"] = tsimg::utils::split(input, ',');

    std::cout << "Formato de exportação ('spice' ou 'gif', padrão: 'spice'): ";
    std::getline(std::cin, input);
    if (!input.empty()) {
        config["export_format"] = input;
    }

    std::cout << "Ativar modo debug? (s/n): ";
    std::getline(std::cin, input);
    bool debug = (input == "s" || input == "S");
    config["debug"] = debug;

    // Configurações avançadas opcionais
    std::cout << "Deseja configurar opções avançadas? (s/n): ";
    std::getline(std::cin, input);
    
    if (input == "s" || input == "S") {
        std::cout << "Caminho da imagem do autor (opcional): ";
        std::getline(std::cin, input);
        if (!input.empty()) {
            config["author_image"] = input;
        }
        
        std::cout << "Texto de ajuda (opcional): ";
        std::getline(std::cin, input);
        
        if (!input.empty()) {
            config["help_text"] = input;
            
            std::cout << "Link de ajuda (opcional): ";
            std::getline(std::cin, input);
            config["help_link"] = input;
            
            std::cout << "URL do ícone da ajuda (opcional): ";
            std::getline(std::cin, input);
            config["help_badge_url"] = input;
        }
        
        std::cout << "Caminho do template personalizado (opcional): ";
        std::getline(std::cin, input);
        if (!input.empty()) {
            config["template"] = input;
        }
    }

    // Processamento usando o novo pipeline
    try {
        tsimg::core::TSIMGPipeline pipeline(debug);
        pipeline.configure(config);
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
    
    try {
        tsimg::utils::ConfigManager configManager;
        if (!configManager.parseCLI(argc, argv)) {
            return 1;
        }
        
        if (configManager.isInfoRequested()) {
            display_info(true);
            return 0;
        }
        
        bool debug = configManager.isDebug();
        tsimg::core::TSIMGPipeline pipeline(debug);
        
        // Configurar pipeline com a configuração consolidada
        pipeline.configure(configManager.getConfig());
        
        // Executar o pipeline
        if (pipeline.execute()) {
            return 0;
        } else {
            return 1;
        }
    } catch (const std::exception& e) {
        tsimg::utils::errorLog(true, e.what());
        display_info(false);
        return 1;
    }
}