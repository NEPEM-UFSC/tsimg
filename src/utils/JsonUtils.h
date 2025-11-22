#pragma once
#include <string>
#include <nlohmann/json.hpp>

namespace tsimg::utils {
    nlohmann::json read_json_file(const std::string& filename, bool debug);
}
