#pragma once
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include <memory>
#include "../generators/IFormatGenerator.h"

namespace tsimg::core {

    class TSIMGPipeline {
    private:
        bool debug;
        std::string format;
        std::string outputFilename;
        std::vector<std::string> imagePaths;
        std::vector<std::string> labels;
        std::string title;
        nlohmann::json config;

    public:
        TSIMGPipeline(bool debug);

        TSIMGPipeline& setFormat(const std::string& fmt);
        TSIMGPipeline& setOutputFilename(const std::string& filename);
        TSIMGPipeline& setImagePaths(const std::vector<std::string>& paths);
        TSIMGPipeline& setLabels(const std::vector<std::string>& lbls);
        TSIMGPipeline& setTitle(const std::string& ttl);
        TSIMGPipeline& setConfig(const nlohmann::json& cfg);
        TSIMGPipeline& configure(const nlohmann::json& cfg);
        TSIMGPipeline& loadFromJsonConfig(const std::string& jsonPath);

        bool validate();
        bool execute();
    };
}
