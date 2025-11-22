#include "JsonUtils.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <stdexcept>

namespace tsimg::utils {
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
}
