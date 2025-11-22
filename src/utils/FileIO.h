#pragma once
#include <string>
#include <vector>

namespace tsimg::utils {
    class FileIO {
    public:
        static std::vector<unsigned char> readBinary(const std::string& filepath);
        static void writeBinary(const std::string& filepath, const std::vector<unsigned char>& data);
        
        static bool copyFile(const std::string& source, const std::string& destination);
        static bool moveFile(const std::string& source, const std::string& destination);
        static bool deleteFile(const std::string& filepath);
        static bool createDirectory(const std::string& dirPath);
    };
}
