#pragma once
#include <string>
#include <iostream>

namespace tsimg::utils {
    void debugLog(bool debug, const std::string& message);
    void errorLog(bool debug, const std::string& message);
}
