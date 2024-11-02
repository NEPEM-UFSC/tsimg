#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION

#include <stb_image.h>
#include <stb_image_resize2.h>
#include "image_utils.h"
#include "gif.h"
#include <iostream>
#include <fstream>


/**
 * @brief Creates a GIF from a sequence of images.
 *
 * This function takes a list of image file paths, resizes each image to the specified width and height,
 * and combines them into a single GIF file.
 *
 * @param output_filename The name of the output GIF file.
 * @param image_paths A vector of strings containing the file paths of the images to be included in the GIF.
 * @param width The width to which each image should be resized.
 * @param height The height to which each image should be resized.
 * @param debug If true, debug information will be printed to the standard output.
 * @return true if the GIF was created successfully, false otherwise.
 */
bool createGif(const std::string& output_filename, const std::vector<std::string>& image_paths, bool debug) {
    if (image_paths.empty()) {
        if (debug) std::cerr << "No images provided." << std::endl;
        return false;
    }

    int width = 0, height = 0, channels = 0;

    // Carregar a primeira imagem para obter as dimensões
    if (debug) std::cout << "Loading first image to get dimensions..." << std::endl;
    unsigned char* first_image = stbi_load(image_paths[0].c_str(), &width, &height, &channels, 4); // Carregar com 4 canais (RGBA)
    if (!first_image) {
        if (debug) std::cerr << "Failed to load image: " << image_paths[0] << std::endl;
        return false;
    }
    stbi_image_free(first_image);
    if (debug) std::cout << "First image loaded successfully. Dimensions: " << width << "x" << height << std::endl;

    GifWriter gif;
    if (!GifBegin(&gif, output_filename.c_str(), width, height, 100)) {
        if (debug) std::cerr << "Failed to initialize GIF: " << output_filename << std::endl;
        return false;
    }

    for (const auto& image_path : image_paths) {
        if (debug) std::cout << "Processing image: " << image_path << std::endl;

        int img_width = 0, img_height = 0, img_channels = 0;
        unsigned char* image_data = stbi_load(image_path.c_str(), &img_width, &img_height, &img_channels, 4);
        if (!image_data) {
            if (debug) std::cerr << "Failed to load image: " << image_path << std::endl;
            GifEnd(&gif);
            return false;
        }

        if (debug) std::cout << "Image loaded successfully. Dimensions: " << img_width << "x" << img_height << std::endl;

        std::vector<uint8_t> resized_image(width * height * 4);
        stbir_resize_uint8_linear(image_data, img_width, img_height, 0, resized_image.data(), width, height, 0, (stbir_pixel_layout)4);

        GifWriteFrame(&gif, resized_image.data(), width, height, 100);

        stbi_image_free(image_data);
    }

    GifEnd(&gif);

    if (debug) std::cout << "GIF created successfully: " << output_filename << std::endl;
    return true;
}


/**
 * @brief Codifica uma imagem em formato Base64.
 * 
 * @param imagePath Caminho para a imagem.
 * @param debug Ativa o modo de depuração para imprimir detalhes da operação.
 * @return A string codificada em Base64 representando a imagem.
 */
std::string encodeImageToBase64(const std::string& imagePath, bool debug) {
    if (debug) {
        std::cout << "Encoding image to Base64: " << imagePath << std::endl;
    }

    std::ifstream file(imagePath, std::ios::binary);
    if (!file.good()) {
        std::cerr << "Could not open file: " << imagePath << std::endl;
        return "";
    }

    std::vector<unsigned char> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    static const char base64Chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string base64;
    int val = 0, valb = -6;
    for (unsigned char c : buffer) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            base64.push_back(base64Chars[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6) base64.push_back(base64Chars[((val << 8) >> (valb + 8)) & 0x3F]);
    while (base64.size() % 4) base64.push_back('=');

    if (debug) {
        std::cout << "Image encoded successfully." << std::endl;
    }

    return base64;
}