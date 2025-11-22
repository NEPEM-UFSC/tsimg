#include "TSIMGPipeline.h"
#include "../generators/SPICEGenerator.h"
#include "../generators/GIFGenerator.h"
#include "../tsimg_spice.h" // For utils
#include <iostream>
#include <filesystem>

namespace tsimg::core {

    TSIMGPipeline::TSIMGPipeline(bool debug) : debug(debug) {
        title = "TSIMG Presentation";
        format = "spice"; // formato padrão
    }

    TSIMGPipeline& TSIMGPipeline::setFormat(const std::string& fmt) {
        format = fmt;
        return *this;
    }

    TSIMGPipeline& TSIMGPipeline::setOutputFilename(const std::string& filename) {
        outputFilename = filename;
        return *this;
    }

    TSIMGPipeline& TSIMGPipeline::setImagePaths(const std::vector<std::string>& paths) {
        imagePaths = paths;
        return *this;
    }

    TSIMGPipeline& TSIMGPipeline::setLabels(const std::vector<std::string>& lbls) {
        labels = lbls;
        return *this;
    }

    TSIMGPipeline& TSIMGPipeline::setTitle(const std::string& ttl) {
        title = ttl.empty() ? "TSIMG Presentation" : ttl;
        return *this;
    }

    TSIMGPipeline& TSIMGPipeline::setConfig(const nlohmann::json& cfg) {
        config = cfg;
        return *this;
    }

    TSIMGPipeline& TSIMGPipeline::configure(const nlohmann::json& cfg) {
        config = cfg;
        
        // Aplicar configurações do JSON
        if (config.contains("export_format")) {
            format = config["export_format"];
        }
        
        if (config.contains("output_filename")) {
            outputFilename = config["output_filename"];
        }
        
        if (config.contains("title")) {
            title = config["title"];
        }
        
        if (config.contains("labels") && config["labels"].is_array()) {
            labels.clear();
            for (const auto& label : config["labels"]) {
                labels.push_back(label);
            }
        }
        if (config.contains("images") && config["images"].is_array()) {
            imagePaths.clear();
            for (const auto& img : config["images"]) {
                imagePaths.push_back(img);
            }
        }
        
        // Processar listas de imagens adicionais (images_1, images_2, etc.)
        for (int i = 1; ; i++) {
            std::string key = "images_" + std::to_string(i);
            if (!config.contains(key) || !config[key].is_array())
                break;
            
            // Criar um vetor para esta lista de imagens
            std::vector<std::string> extraImageList;
            for (const auto& img : config[key]) {
                extraImageList.push_back(img);
            }
            
            // Se não existir a lista extraImageLists no config, criá-la
            if (!config.contains("extraImageLists")) {
                config["extraImageLists"] = nlohmann::json::array();
            }
            
            // Adicionar esta lista ao config
            config["extraImageLists"].push_back(extraImageList);
        }
        return *this;
    }

    TSIMGPipeline& TSIMGPipeline::loadFromJsonConfig(const std::string& jsonPath) {
        try {
            nlohmann::json loadedConfig = tsimg::utils::read_json_file(jsonPath, debug);
            return configure(loadedConfig);
        } catch (const std::exception& e) {
            tsimg::utils::errorLog(true, "Error loading JSON config: " + std::string(e.what()));
            throw;
        }
    }

    bool TSIMGPipeline::validate() {
        // Validar formato
        if (format != "spice" && format != "gif") {
            tsimg::utils::errorLog(true, "Unsupported format: " + format);
            return false;
        }
        
        // Validar nome do arquivo de saída
        if (outputFilename.empty()) {
            tsimg::utils::errorLog(true, "Output filename cannot be empty");
            return false;
        }
        
        // Validar caminhos de imagem
        if (imagePaths.empty()) {
            tsimg::utils::errorLog(true, "No image paths provided");
            return false;
        }
        
        for (const auto& img : imagePaths) {
            if (!tsimg::utils::ImageValidator::validateImagePath(img, debug)) {
                tsimg::utils::errorLog(true, "Invalid image file: " + img);
                return false;
            }
        }
        
        return true;
    }

    bool TSIMGPipeline::execute() {
        if (!validate()) {
            return false;
        }
        
        try {
            std::unique_ptr<tsimg::generators::IFormatGenerator> generator;
            
            if (format == "spice") {
                generator = std::make_unique<tsimg::generators::SPICEGenerator>(debug);
            } else if (format == "gif") {
                generator = std::make_unique<tsimg::generators::GIFGenerator>(debug);
            } else {
                throw std::runtime_error("Unsupported format: " + format);
            }
            
            // Executar a geração
            return generator->generate(outputFilename, imagePaths, labels, title, config);
        } catch (const std::exception& e) {
            tsimg::utils::errorLog(true, "Error in pipeline execution: " + std::string(e.what()));
            return false;
        }
    }
}
