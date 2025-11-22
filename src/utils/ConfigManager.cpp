#include "ConfigManager.h"
#include "JsonUtils.h"
#include "Logger.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

namespace tsimg::utils {

ConfigManager::ConfigManager() {
    // Default configuration
    config["title"] = "TSIMG Presentation";
    config["export_format"] = "spice";
    config["debug"] = false;
}

bool ConfigManager::parseCLI(int argc, char* argv[]) {
    // First pass: check for debug and config file
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-debug") {
            debug = true;
            config["debug"] = true;
        } else if (arg == "-config" && i + 1 < argc) {
            loadJsonConfig(argv[++i]);
        } else if (arg == "-info") {
            infoRequested = true;
        }
    }

    // Second pass: override with CLI arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "-n" && i + 1 < argc) {
            config["output_filename"] = argv[++i];
        } else if (arg == "-i" && i + 1 < argc) {
            std::vector<std::string> images = split(argv[++i], ',');
            config["images"] = images;
        } else if (arg == "-l" && i + 1 < argc) {
            std::vector<std::string> labels = split(argv[++i], ',');
            config["labels"] = labels;
        } else if (arg == "-f" && i + 1 < argc) {
            config["export_format"] = argv[++i];
        } else if (arg == "-labelbyname") {
            config["createLabelsFromImages"] = true;
        } else if (arg == "-authorimage" && i + 1 < argc) {
            config["author_image"] = argv[++i];
        } else if (arg == "-help_text" && i + 1 < argc) {
            config["help_text"] = argv[++i];
        } else if (arg == "-help_link" && i + 1 < argc) {
            config["help_link"] = argv[++i];
        } else if (arg == "-help_badge_url" && i + 1 < argc) {
            config["help_badge_url"] = argv[++i];
        } else if (arg == "-title" && i + 1 < argc) {
            config["title"] = argv[++i];
        } else if (arg == "-template" && i + 1 < argc) {
            config["template"] = argv[++i];
        } else if ((arg == "-2" || arg == "-3") && i + 1 < argc) {
             // Handle -2, -3 for extra image lists
             if (!config.contains("extraImageLists")) {
                 config["extraImageLists"] = nlohmann::json::array();
             }
             config["extraImageLists"].push_back(split(argv[++i], ','));
        }
    }
    
    return true;
}

void ConfigManager::loadJsonConfig(const std::string& filepath) {
    try {
        nlohmann::json jsonConfig = read_json_file(filepath, debug);
        config.update(jsonConfig);
    } catch (const std::exception& e) {
        errorLog(true, "Failed to load config file: " + std::string(e.what()));
        throw;
    }
}

std::vector<std::string> ConfigManager::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream token_stream(str);
    while (std::getline(token_stream, token, delimiter)) {
        // Trim spaces
        size_t first = token.find_first_not_of(' ');
        if (std::string::npos == first) {
            continue;
        }
        size_t last = token.find_last_not_of(' ');
        tokens.push_back(token.substr(first, (last - first + 1)));
    }
    return tokens;
}

nlohmann::json ConfigManager::getConfig() const {
    return config;
}

bool ConfigManager::isInfoRequested() const {
    return infoRequested;
}

bool ConfigManager::isDebug() const {
    return debug;
}

}
