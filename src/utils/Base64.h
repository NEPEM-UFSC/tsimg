#pragma once
#include <string>
#include <vector>

namespace tsimg::utils {
    class Base64 {
    public:
        static std::string encode(const std::vector<unsigned char>& data);
        static std::vector<unsigned char> decode(const std::string& encodedData);
        static std::string encodeFromFile(const std::string& filepath);
    };
}
