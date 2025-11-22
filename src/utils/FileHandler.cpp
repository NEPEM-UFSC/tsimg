#include "FileHandler.h"
#include "Logger.h"
#include <filesystem>
#include <vector>
#include <algorithm>
#include <stdexcept>

namespace tsimg::utils {

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
}
