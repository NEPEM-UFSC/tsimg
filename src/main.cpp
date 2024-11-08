#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <nlohmann/json.hpp>
#include "tsimg.h"
#include "image_utils.h"
#include "spice_object.h"
#include "template_writer.h"

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream token_stream(str);
    while (std::getline(token_stream, token, delimiter)) {
        // Remove leading and trailing whitespace
        token.erase(0, token.find_first_not_of(' '));
        token.erase(token.find_last_not_of(' ') + 1);
        tokens.push_back(token);
    }
    return tokens;
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

std::string help_text = "Mais informações disponíveis no botão abaixo.";
std::string helpBadgeURL = "data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHdpZHRoPSIyNCIgaGVpZ2h0PSIyNCIgdmlld0JveD0iMCAwIDI0IDI0IiBmaWxsPSJub25lIiBzdHJva2U9ImN1cnJlbnRDb2xvciIgc3Ryb2tlLXdpZHRoPSIyIiBzdHJva2UtbGluZWNhcD0icm91bmQiIHN0cm9rZS1saW5lam9pbj0icm91bmQiIGNsYXNzPSJsdWNpZGUgbHVjaWRlLWNpcmNsZS1oZWxwIj48Y2lyY2xlIGN4PSIxMiIgY3k9IjEyIiByPSIxMCIvPjxwYXRoIGQ9Ik05LjA5IDlhMyAzIDAgMCAxIDUuODMgMWMwIDItMyAzLTMgMyIvPjxwYXRoIGQ9Ik0xMiAxN2guMDEiLz48L3N2Zz4=";
std::string helpLink = "https://github.com/NEPEM-UFSC/tsimg";

void display_info() {
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
    std::cout << "  Temporal Series Interactive Imager (TSIMG)       " << std::endl;
    std::cout << "===================================================" << std::endl;
    std::cout << "\nA tool for creating interactive images of time series." << std::endl;
    std::cout << "Supports export to SPICE and GIF formats." << std::endl;
    std::cout << "For more information, please visit: \nhttps://github.com/NEPEM-UFSC/tsimg" << std::endl;
    std::cout << "\n===================================================\n" << std::endl;
}
int main(int argc, char* argv[]) {
    std::string output_filename;
    std::vector<std::string> image_paths;
    std::vector<std::string> labels;
    bool debug = false;
    bool createLabelsFromImages = false;
    std::string format = "spice";  // Default format is "spice"
    std::string json_config_file;
    std::string author_image_path;

    if (argc == 1) {
        display_info();
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
        return 1;
    }

    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "-info") == 0) {
            display_info();
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
        }
    }

    if (!json_config_file.empty()) {
        try {
            // Lê o arquivo JSON de configuração
            nlohmann::json config = read_json_file(json_config_file, debug);
            
            // Extrai valores do JSON, usando valores padrão se as chaves não estiverem presentes
            format = config.value("export_format", "spice");
            output_filename = config.value("output_filename", "output.html");
            image_paths = config.value("images", std::vector<std::string>{});
            labels = config.value("labels", std::vector<std::string>{});
            std::string title = config.value("title", "SPICE Presentation");
            std::string help_text = config.value("help_text", "Mais informações disponíveis no botão abaixo.");
            std::string help_link = config.value("help_link", "https://github.com/NEPEM-UFSC/tsimg");
            std::string help_badge_url = config.value("help_badge_url", "data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHdpZHRoPSIyNCIgaGVpZ2h0PSIyNCIgdmlld0JveD0iMCAwIDI0IDI0IiBmaWxsPSJub25lIiBzdHJva2U9ImN1cnJlbnRDb2xvciIgc3Ryb2tlLXdpZHRoPSIyIiBzdHJva2UtbGluZWNhcD0icm91bmQiIHN0cm9rZS1saW5lam9pbj0icm91bmQiIGNsYXNzPSJsdWNpZGUgbHVjaWRlLWNpcmNsZS1oZWxwIj48Y2lyY2xlIGN4PSIxMiIgY3k9IjEyIiByPSIxMCIvPjxwYXRoIGQ9Ik05LjA5IDlhMyAzIDAgMCAxIDUuODMgMWMwIDItMyAzLTMgMyIvPjxwYXRoIGQ9Ik0xMiAxN2guMDEiLz48L3N2Zz4=");
            std::string author_image = config.value("author_image", "");
            std::string main_text = config.value("main_text", "This is generated from a JSON config.");
    
            // Verifica o formato de exportação
            if (format == "gif") {
                // Cria um arquivo GIF
                if (!createGif(output_filename, image_paths, debug)) {
                    std::cerr << "Failed to create GIF file: " << output_filename << std::endl;
                    return 1;
                }
            }
             else if (format == "spice") {
                // Cria um objeto SPICEBuilder e adiciona conteúdo, imagens, rótulos e configurações de ajuda
                SPICEBuilder builder(title, debug);
                builder.addTitle(title);
                builder.addContent("SPICE_TEXT", main_text);
                for (const auto& img : image_paths) {
                    builder.addImage(img);
                }
                if (createLabelsFromImages) {
                    builder.generateLabelsFromImages();
                }
                builder.addLabels(labels);
                builder.setHelp(help_text, help_link, help_badge_url);
                if (!author_image.empty()) {
                    builder.setAuthorImage(author_image);
                }
                // Gera o arquivo SPICE a partir do template HTML
                TemplateWriter writer("template.html", debug);
                writer.build(builder, output_filename);
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
            return 1;
        }

        if (format == "spice") {
            SPICEBuilder builder("CLI Title", debug);
            builder.addTitle("CLI Title");
            for (const auto& img : image_paths) {
                builder.addImage(img);
            }
            if (createLabelsFromImages) {
                builder.generateLabelsFromImages();
            }
            builder.addLabels(labels);
            builder.setHelp(help_text, helpLink, helpBadgeURL);
            if (!author_image_path.empty()) {
                builder.setAuthorImage(author_image_path);
            }
            TemplateWriter writer("template.html", debug);
            writer.build(builder, output_filename);
        } else if (format == "gif") {
            if (!createGif(output_filename, image_paths, debug)) {
                std::cerr << "Falha ao criar o arquivo GIF: " << output_filename << std::endl;
                return 1;
            }
        } else {
            std::cerr << "Unsupported format: " << format << std::endl;
            return 1;
        }
    }
    return 0;
}