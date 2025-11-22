#include "Base64.h"
#include "FileIO.h"
#include <cstdint>

namespace tsimg::utils {

    std::string Base64::encode(const std::vector<unsigned char>& data) {
        static const char* encoding_table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        std::string encoded;
        encoded.reserve(((data.size() + 2) / 3) * 4);

        for (size_t i = 0; i < data.size(); i += 3) {
            uint32_t octet_a = i < data.size() ? data[i] : 0;
            uint32_t octet_b = i + 1 < data.size() ? data[i + 1] : 0;
            uint32_t octet_c = i + 2 < data.size() ? data[i + 2] : 0;

            uint32_t triple = (octet_a << 16) + (octet_b << 8) + octet_c;

            encoded.push_back(encoding_table[(triple >> 18) & 0x3F]);
            encoded.push_back(encoding_table[(triple >> 12) & 0x3F]);
            encoded.push_back(encoding_table[(triple >> 6) & 0x3F]);
            encoded.push_back(encoding_table[triple & 0x3F]);
        }

        int mod_table[] = {0, 2, 1};
        int padding = mod_table[data.size() % 3];
        for (int i = 0; i < padding; i++) {
            encoded[encoded.size() - 1 - i] = '=';
        }

        return encoded;
    }

    std::vector<unsigned char> Base64::decode(const std::string& encodedData) {
        // Implementação futura
        return {};
    }

    std::string Base64::encodeFromFile(const std::string& filepath) {
        auto data = FileIO::readBinary(filepath);
        return encode(data);
    }
}
