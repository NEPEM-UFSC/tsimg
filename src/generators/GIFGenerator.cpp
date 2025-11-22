#include "GIFGenerator.h"
#include "../tsimg_spice.h" // For debugLog/errorLog

namespace tsimg::generators {

    GIFGenerator::GIFGenerator(bool debug) : debug(debug) {}

    bool GIFGenerator::generate(const std::string& outputFilename, 
                  const std::vector<std::string>& imagePaths,
                  const std::vector<std::string>& labels, // Unused in GIFGenerator
                  const std::string& title, // Unused in GIFGenerator
                  const nlohmann::json& config) {
        try {
            // Configurar parâmetros do GIF
            int delay = 100; // padrão
            if (config.contains("gif_delay")) {
                delay = config["gif_delay"].get<int>();
            }
            
            bool loop = true; // padrão
            if (config.contains("gif_loop")) {
                loop = config["gif_loop"].get<bool>();
            }
            
            int quality = 75; // padrão
            if (config.contains("gif_quality")) {
                quality = config["gif_quality"].get<int>();
            }
            
            if (createGif(outputFilename, imagePaths, debug, delay, loop, quality)) {
                tsimg::utils::debugLog(true, "GIF file generated successfully: " + outputFilename);
                return true;
            } else {
                return false;
            }
        } catch (const std::exception& e) {
            tsimg::utils::errorLog(true, "Error creating GIF: " + std::string(e.what()));
            return false;
        }
    }
}
