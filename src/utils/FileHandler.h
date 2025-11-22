#pragma once
#include <string>
#include <fstream>

namespace tsimg::utils {
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
}
