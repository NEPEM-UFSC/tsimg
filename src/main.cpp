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

const std::string DEFAULT_TITLE = "TSIMG Presentation";
const std::string APP_NAME = "Temporal Series Interactive Imager";

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

std::string concatenateStrings(const std::vector<std::string>& vec) {
    std::ostringstream oss;
    for (const auto& str : vec) {
        oss << str;
    }
    return oss.str();
}

std::string readFileToString(const std::string& filepath, bool debug) {
    if (debug) std::cout << "Reading file: " << filepath << std::endl;
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error while trying to open the template file: " << filepath << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    
    if (debug) std::cout << "File read successfully." << std::endl;
    return buffer.str();
}

nlohmann::json read_json_file(const std::string& filename, bool debug) {
    if (debug) {
        std::cout << "Reading JSON config file: " << filename << std::endl;
    }
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open JSON file: " + filename);
    }

    nlohmann::json json_data;
    file >> json_data;

    if (debug) {
        std::cout << "JSON file read successfully: " << filename << std::endl;
    }
    return json_data;
}

std::string help_text;
std::string help_badge_url;
std::string help_link;

bool app_info = false;

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
        std::cout << " \nTemporal Series Interactive Imager (TSIMG)\n" << std::endl;
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
    std::cerr << "  --config <config.json>   Path to JSON config file (optional)." << std::endl;
    std::cerr << "  --labelbyname           Generate labels from image names (optional)." << std::endl;
    std::cerr << "  -author_image <author_image_path>   Path to author image (optional)." << std::endl;
    std::cerr << "  -help_text <text>       Help text to display (optional)." << std::endl;
    std::cerr << "  -help_link <link>       Help link URL (optional)." << std::endl;
    std::cerr << "  -help_badge_url <url>   Help badge image URL (optional)." << std::endl;
    std::cerr << "  -template <template_path> Path to custom HTML template (optional)." << std::endl;
}

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

int main(int argc, char* argv[]) {
    if (argc == 1) {
        // Modo Interativo
        app_info = true;
        display_info();

        std::cout << "Por favor, insira os parâmetros necessários." << std::endl;

        std::string output_filename;
        std::string images_input;
        std::string labels_input;
        std::string format = "spice";
        bool debug = false;
        std::string json_config_file;
        std::string title;  // Novo campo para título

        // Adicionar prompt para título
        std::cout << "Título da apresentação (pressione Enter para usar o padrão): ";
        std::getline(std::cin, title);
        if (title.empty()) {
            title = DEFAULT_TITLE;
        }

        std::cout << "Nome do arquivo de saída: ";
        std::getline(std::cin, output_filename);

        std::cout << "Caminhos das imagens (separados por vírgula): ";
        std::getline(std::cin, images_input);
        std::vector<std::string> image_paths = split(images_input, ',');

        std::cout << "Labels (opcional, separados por vírgula): ";
        std::getline(std::cin, labels_input);
        std::vector<std::string> labels = split(labels_input, ',');

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

        // Processamento dos dados de entrada
        try {
            if (format == "spice") {
                SPICEBuilder builder(title, debug);
                builder.addTitle(title);  // Usar o mesmo título
                builder.addImagesAsync(image_paths);  // Usar a versão assíncrona
                builder.addLabels(labels);
                TemplateWriter writer("template_vs.html", debug);
                writer.writeToFile(output_filename, builder.getContents(), builder.getImageLists(), builder.getLabels(), builder.getAuthorImageBase64());
                std::cout << "Arquivo SPICE gerado com sucesso: " << output_filename << std::endl;
            } else if (format == "gif") {
                if (createGif(output_filename, image_paths, debug)) {
                    std::cout << "Arquivo GIF gerado com sucesso: " << output_filename << std::endl;
                } else {
                    std::cerr << "Falha ao criar o arquivo GIF: " << output_filename << std::endl;
                }
            } else {
                std::cerr << "Formato não suportado: " << format << std::endl;
                return 1;
            }
        } catch (const std::exception& e) {
            std::cerr << "Erro durante o processamento: " << e.what() << std::endl;
            return 1;
        }

        return 0;
    }
    std::string output_filename;
    std::vector<std::string> image_paths;
    std::vector<std::string> labels;
    bool debug = false;
    bool createLabelsFromImages = false;
    std::string format = "spice";
    std::string json_config_file;
    std::string author_image_path;
    std::string template_path;

    std::vector<std::vector<std::string>> imagePathsExtras;

    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "-info") == 0) {
            app_info=true;
            display_info();
            app_info=false;
            return 0;
        } else if (std::strcmp(argv[i], "-n") == 0 && i + 1 < argc) {
            output_filename = argv[++i];
        } else if (std::strcmp(argv[i], "-i") == 0 && i + 1 < argc) {
            image_paths = split(argv[++i], ',');
        } else if (std::strcmp(argv[i], "-l") == 0 && i + 1 < argc) {
            labels = split(argv[++i], ',');
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
        } else if (std::strncmp(argv[i], "-2", 2) == 0 && i + 1 < argc) {
            imagePathsExtras.push_back(split(argv[++i], ','));
        } else if (std::strncmp(argv[i], "-3", 2) == 0 && i + 1 < argc) {
            imagePathsExtras.push_back(split(argv[++i], ','));
        } else if (std::strcmp(argv[i], "-template") == 0 && i + 1 < argc) {
            template_path = argv[++i];
        }
    }

    if (!json_config_file.empty()) {
        try {
            nlohmann::json config = read_json_file(json_config_file, debug);
            
            if (!validateJsonConfig(config, debug)) {
                std::cerr << "Invalid JSON configuration file" << std::endl;
                return 1;
            }
            
            format = config.value("export_format", "spice");
            output_filename = config.value("output_filename", "output.html");
            labels = config.value("labels", std::vector<std::string>{});
            std::string title = config.value("title", DEFAULT_TITLE);  // Usar título padrão
            std::string main_text = config.value("main_text", "This is generated from a JSON config.");

            help_text = config.value("help_text", "");
            help_link = config.value("help_link", "");
            help_badge_url = config.value("help_badge_url", "");
            std::string author_image = config.value("author_image", "");
            std::string template_file = config.value("template", "");

            std::map<std::string, std::unique_ptr<ImageList>> imageLists;
            for (int i = 0; ; ++i) {
                std::string key = "images" + (i == 0 ? "" : "_" + std::to_string(i));
                if (config.contains(key)) {
                    std::string placeholder = "SPICE_IMAGES" + (i == 0 ? "" : "_" + std::to_string(i));
                    auto imageList = std::make_unique<ImageList>();
                    for (const auto& img : config[key]) {
                        if (debug) std::cout << "Processing image: " << img << std::endl;
                        imageList->addImage(std::make_unique<Image>(img, ""));
                        if (debug) std::cout << "Image added successfully: " << img << std::endl;
                    }
                    imageLists[placeholder] = std::move(imageList);
                } else {
                    break;
                }
            }

            if (format == "gif") {
                if (!createGif(output_filename, image_paths, debug)) {
                    std::cerr << "Failed to create GIF file: " << output_filename << std::endl;
                    return 1;
                }
            } else if (format == "spice") {
                SPICEBuilder builder(title, debug);
                builder.addTitle(title);  // Consistência no uso do título
                builder.addContent("SPICE_TEXT", main_text);
                if (createLabelsFromImages) {
                    builder.generateLabelsFromImages();
                }
                builder.addLabels(labels);
                if (!help_text.empty() && !help_link.empty() && !help_badge_url.empty()) {
                    builder.setHelp(help_text, help_link, help_badge_url);
                }
                if (!author_image.empty()) {
                    builder.setAuthorImage(author_image);
                }
                if (!template_file.empty()) {
                    builder.setTemplate(template_file);
                }
                for (const auto& [tag, list] : imageLists) {
                    for (const auto& img : list->getImages()) {
                        builder.addImageToList(tag, img->getPath());
                    }
                }
                TemplateWriter writer("template_vs.html", debug);
                writer.writeToFile(output_filename, builder.getContents(), builder.getImageLists(), builder.getLabels(), builder.getAuthorImageBase64());
            } else {
                std::cerr << "Unsupported format in JSON config: " << format << std::endl;
                return 1;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error reading or processing JSON config file: " << e.what() << std::endl;
            return 1;
        }
    } else {
        if (output_filename.empty() || image_paths.empty()) {
            display_info();
            return 1;
        }

        // Validar caminhos de imagem em modo CLI
        for (const auto& img : image_paths) {
            if (!tsimg::utils::ImageValidator::validateImagePath(img, debug)) {
                std::cerr << "Invalid image file: " << img << std::endl;
                return 1;
            }
        }
        
        // Validar caminhos de imagem extras
        for (const auto& extraList : imagePathsExtras) {
            for (const auto& img : extraList) {
                if (!tsimg::utils::ImageValidator::validateImagePath(img, debug)) {
                    std::cerr << "Invalid image file in extra list: " << img << std::endl;
                    return 1;
                }
            }
        }

        if (format == "spice") {
            SPICEBuilder builder(DEFAULT_TITLE, debug);  // Usar título padrão
            builder.addTitle(DEFAULT_TITLE);
            builder.addImagesAsync(image_paths);  // Usar a versão assíncrona
            if (createLabelsFromImages) {
                builder.generateLabelsFromImages();
            }
            builder.addLabels(labels);
            if (!help_text.empty() && !help_link.empty() && !help_badge_url.empty()) {
                builder.setHelp(help_text, help_link, help_badge_url);
            }
            if (!author_image_path.empty()) {
                builder.setAuthorImage(author_image_path);
            }
            if (!template_path.empty()) {
                builder.setTemplate(template_path);
            }
            for (size_t i = 0; i < imagePathsExtras.size(); ++i) {
                std::string tag = "SPICE_IMAGES_" + std::to_string(i + 1);
                for (const auto& img : imagePathsExtras[i]) {
                    builder.addImageToList(tag, img);
                }
            }
            TemplateWriter writer("template_vs.html", debug);
            writer.writeToFile(output_filename, builder.getContents(), builder.getImageLists(), builder.getLabels(), builder.getAuthorImageBase64());
        } else if (format == "gif") {
            if (!createGif(output_filename, image_paths, debug)) {
                std::cerr << "Error while trying to create the gif file: " << output_filename << std::endl;
                return 1;
            }
        } else {
            std::cerr << "Unsupported format: " << format << std::endl;
            return 1;
        }
    }
    return 0;
}