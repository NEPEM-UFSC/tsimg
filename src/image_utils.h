#pragma once

#include <string>
#include <vector>

// Função para criar um GIF a partir de várias imagens
bool createGif(const std::string& output_filename, const std::vector<std::string>& image_paths, bool debug = false);

std::string encodeImageToBase64(const std::string& imagePath, bool debug);
