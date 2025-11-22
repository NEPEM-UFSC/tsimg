#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <optional>

// Forward declarations
class Image;

/**
 * @brief Cria um arquivo GIF a partir de uma sequência de imagens.
 * 
 * @param outputFilename Nome do arquivo de saída
 * @param imagePaths Caminhos das imagens de entrada
 * @param debug Flag para ativar modo de depuração
 * @param delay Tempo entre frames em milisegundos (padrão: 100ms)
 * @param loop Flag para indicar se o GIF deve repetir infinitamente (padrão: true)
 * @param quality Qualidade da imagem (0-100, padrão: 75)
 * @return true se a operação foi bem-sucedida
 * @return false caso contrário
 */
bool createGif(
    const std::string& outputFilename,
    const std::vector<std::string>& imagePaths,
    bool debug = false,
    int delay = 100,
    bool loop = true,
    int quality = 75
);

/**
 * @brief Classe para configuração e geração de arquivos GIF.
 */
class GIFBuilder {
public:
    GIFBuilder(bool debug = false);
    ~GIFBuilder();

    // Métodos de configuração
    GIFBuilder& setOutputFilename(const std::string& filename);
    GIFBuilder& addImage(const std::string& imagePath);
    GIFBuilder& addImages(const std::vector<std::string>& imagePaths);
    GIFBuilder& setDelay(int milliseconds);
    GIFBuilder& setLoop(bool shouldLoop);
    GIFBuilder& setQuality(int qualityLevel);
    GIFBuilder& setResizeDimensions(int width, int height);
    GIFBuilder& setTransparentColor(int r, int g, int b);

    // Métodos avançados
    GIFBuilder& addTextOverlay(const std::string& text, int x, int y, const std::string& fontPath = "");
    GIFBuilder& addImageOverlay(const std::string& imagePath, int x, int y, float opacity = 1.0f);
    GIFBuilder& addFrameCallback(std::function<void(Image&, int)> callback);
    GIFBuilder& setOptimization(bool optimize);

    // Executar a geração
    bool build();
    bool saveToFile();

private:
    bool debug;
    std::string outputFilename;
    std::vector<std::string> imagePaths;
    int delay;
    bool loop;
    int quality;
    std::optional<std::pair<int, int>> resizeDimensions;
    std::optional<std::tuple<int, int, int>> transparentColor;
    std::vector<std::tuple<std::string, int, int, std::string>> textOverlays;
    std::vector<std::tuple<std::string, int, int, float>> imageOverlays;
    std::vector<std::function<void(Image&, int)>> frameCallbacks;
    bool optimize;
};

/**
 * @brief Classe para extração de frames de um GIF existente.
 */
class GIFReader {
public:
    GIFReader(const std::string& gifPath, bool debug = false);
    ~GIFReader();

    // Métodos de informação
    int getFrameCount() const;
    std::pair<int, int> getDimensions() const;
    int getDelay() const;
    bool isAnimated() const;

    // Métodos de extração
    std::vector<std::unique_ptr<Image>> extractAllFrames();
    std::unique_ptr<Image> extractFrame(int frameIndex);
    bool saveFrameToFile(int frameIndex, const std::string& outputPath);
    bool saveAllFramesToDirectory(const std::string& outputDirectory, const std::string& prefix = "frame_");

private:
    std::string gifPath;
    bool debug;
    int frameCount;
    std::pair<int, int> dimensions;
    int delay;
    bool animated;
    bool isValid;
};

/**
 * @brief Classe para otimizar arquivos GIF.
 */
class GIFOptimizer {
public:
    GIFOptimizer(const std::string& inputPath, bool debug = false);
    ~GIFOptimizer();

    // Métodos de otimização
    GIFOptimizer& reduceColors(int maxColors);
    GIFOptimizer& removeRedundantFrames(float threshold = 0.95f);
    GIFOptimizer& reduceSize(int maxWidth, int maxHeight);
    GIFOptimizer& compressFrames(int compressionLevel);

    // Executar a otimização
    bool optimize();
    bool saveToFile(const std::string& outputPath);

private:
    std::string inputPath;
    bool debug;
    int maxColors;
    float redundancyThreshold;
    std::optional<std::pair<int, int>> maxDimensions;
    int compressionLevel;
};

/**
 * @brief Funções utilitárias para GIF
 */
namespace tsimg::gif {
    // Informações sobre GIF
    bool isGif(const std::string& filePath);
    std::pair<int, int> getGifDimensions(const std::string& filePath);
    int getGifFrameCount(const std::string& filePath);
    int getGifDuration(const std::string& filePath);

    // Conversão entre formatos
    bool convertToGif(const std::string& inputPath, const std::string& outputPath, int quality = 75);
    bool convertFromGif(const std::string& gifPath, const std::string& outputPath, const std::string& format = "png");

    // Manipulação de GIF
    bool resizeGif(const std::string& inputPath, const std::string& outputPath, int width, int height);
    bool mergeGifs(const std::vector<std::string>& inputPaths, const std::string& outputPath);
    bool splitGif(const std::string& inputPath, const std::string& outputDirectory);
    bool addWatermark(const std::string& gifPath, const std::string& watermarkPath, const std::string& outputPath);
}
