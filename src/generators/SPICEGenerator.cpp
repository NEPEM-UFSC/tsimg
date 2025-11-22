#include "SPICEGenerator.h"
#include <iostream>

namespace tsimg::generators {

    SPICEGenerator::SPICEGenerator(bool debug) : debug(debug) {}

    bool SPICEGenerator::generate(const std::string& outputFilename, 
                  const std::vector<std::string>& imagePaths,
                  const std::vector<std::string>& labels,
                  const std::string& title,
                  const nlohmann::json& config) {
        try {
            // Criar builder e configurar
            SPICEBuilder builder(title, debug);
            builder.addTitle(title);
            builder.addImagesAsync(imagePaths);
            
            // Adicionar labels
            if (config.contains("createLabelsFromImages") && config["createLabelsFromImages"].get<bool>()) {
                builder.generateLabelsFromImages();
            }
            builder.addLabels(labels);
            
            // Configurar help se disponível
            // TODO: Remove global variables dependency
            // extern std::string help_text, help_link, help_badge_url; 
            // For now, we read from config if available, or ignore globals to enforce clean architecture
            
            if (config.contains("help_text") && config.contains("help_link") && config.contains("help_badge_url")) {
                 builder.setHelp(config["help_text"], config["help_link"], config["help_badge_url"]);
            }
            
            // Configurar imagem do autor se disponível
            if (config.contains("author_image")) {
                builder.setAuthorImage(config["author_image"]);
            }
            
            // Configurar template se disponível
            if (config.contains("template")) {
                builder.setTemplate(config["template"]);
            }
              // Adicionar conteúdo principal se disponível
            if (config.contains("main_text")) {
                builder.addContent("SPICE_TEXT", config["main_text"]);
            }
            
            // Garantir que as imagens da lista principal são adicionadas a SPICE_IMAGES
            if (config.contains("images") && config["images"].is_array()) {
                for (const auto& img : config["images"]) {
                    builder.addImageToList("SPICE_IMAGES", img);
                }
            }
              
            // Processar imagens extras em listas separadas
            if (config.contains("extraImageLists") && config["extraImageLists"].is_array()) {
                int index = 1;
                for (const auto& imageList : config["extraImageLists"]) {
                    if (imageList.is_array()) {
                        std::string tag = "SPICE_IMAGES_" + std::to_string(index++);
                        for (const auto& img : imageList) {
                            builder.addImageToList(tag, img);
                        }
                    }
                }
            }
            
            // Processar também diretamente as listas images_1, images_2, etc.
            for (int i = 1; ; i++) {
                std::string key = "images_" + std::to_string(i);
                if (!config.contains(key) || !config[key].is_array())
                    break;
                
                std::string tag = "SPICE_IMAGES_" + std::to_string(i);
                for (const auto& img : config[key]) {
                    builder.addImageToList(tag, img);
                }
            }
            
            // Escrever arquivo final
            TemplateWriter writer(builder.getTemplatePath(), debug);
            writer.writeToFile(outputFilename, 
                              builder.getContents(), 
                              builder.getImageLists(), 
                              builder.getLabels(), 
                              builder.getAuthorImageBase64());
            
            tsimg::utils::debugLog(true, "SPICE file generated successfully: " + outputFilename);
            return true;
        } catch (const std::exception& e) {
            tsimg::utils::errorLog(true, "Failed to create SPICE file: " + std::string(e.what()));
            return false;
        }
    }
}
