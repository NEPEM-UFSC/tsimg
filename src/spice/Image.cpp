#include "Image.h"

Image::Image(const std::string& path, const std::string& base64Data)
    : path(path), base64Data(base64Data) {}

const std::string& Image::getPath() const {
    return path;
}

std::string Image::toBase64() const {
    return base64Data;
}

Image::~Image() = default;

ImageList::ImageList() = default;
ImageList::~ImageList() = default;

void ImageList::addImage(std::unique_ptr<Image> image) {
    images.push_back(std::move(image));
}

std::vector<std::unique_ptr<Image>>& ImageList::getImages() {
    return images;
}

std::string ImageList::generateImageTags() const {
    std::string imageTags = "";
    for (const auto& image : images) {
        imageTags += "<img src=\"data:image/png;base64," + image->toBase64() + "\" alt=\"" + image->getPath() + "\" loading=\"lazy\">";
    }
    return imageTags;
}
