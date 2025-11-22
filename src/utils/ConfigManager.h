#pragma once
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

namespace tsimg::utils {

class ConfigManager {
public:
    ConfigManager();
    
    // Parse command line arguments and update internal config
    bool parseCLI(int argc, char* argv[]);
    
    // Get the consolidated configuration
    nlohmann::json getConfig() const;
    
    // Helper to check if help/info was requested
    bool isInfoRequested() const;
    
    // Helper to check if debug is enabled
    bool isDebug() const;

private:
    nlohmann::json config;
    bool infoRequested = false;
    bool debug = false;

    // Helper to parse comma-separated lists
    std::vector<std::string> split(const std::string& str, char delimiter);
    
    // Helper to load JSON file
    void loadJsonConfig(const std::string& filepath);
};

}
