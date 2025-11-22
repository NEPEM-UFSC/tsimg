#include "Logger.h"

namespace tsimg::utils {
    void debugLog(bool debug, const std::string& message) {
        if (debug) std::cout << "[DEBUG] " << message << std::endl;
    }

    void errorLog(bool debug, const std::string& message) {
        if (debug) std::cerr << "[ERROR] " << message << std::endl;
    }
}
