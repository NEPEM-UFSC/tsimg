#pragma once

#include <string>
#include <vector>

bool createGif(const std::string& output_filename, const std::vector<std::string>& image_paths, bool debug = false);
std::string encodeImageToBase64(const std::string& imagePath, bool debug);
