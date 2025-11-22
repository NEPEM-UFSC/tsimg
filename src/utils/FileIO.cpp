#include "FileIO.h"
#include <fstream>
#include <stdexcept>

namespace tsimg::utils {

    std::vector<unsigned char> FileIO::readBinary(const std::string& filepath) {
        std::ifstream file(filepath, std::ios::binary | std::ios::ate);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file: " + filepath);
        }

        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        std::vector<unsigned char> buffer(size);
        if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
            throw std::runtime_error("Error reading file: " + filepath);
        }

        return buffer;
    }

    void FileIO::writeBinary(const std::string& filepath, const std::vector<unsigned char>& data) {
        std::ofstream file(filepath, std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file for writing: " + filepath);
        }

        file.write(reinterpret_cast<const char*>(data.data()), data.size());
        if (file.fail()) {
            throw std::runtime_error("Failed to write file: " + filepath);
        }
    }

    bool FileIO::copyFile(const std::string& source, const std::string& destination) {
        // Implementação futura
        return false;
    }

    bool FileIO::moveFile(const std::string& source, const std::string& destination) {
        // Implementação futura
        return false;
    }

    bool FileIO::deleteFile(const std::string& filepath) {
        // Implementação futura
        return false;
    }

    bool FileIO::createDirectory(const std::string& dirPath) {
        // Implementação futura
        return false;
    }
}
