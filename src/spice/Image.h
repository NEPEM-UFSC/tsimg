#pragma once
#include <string>
#include <vector>
#include <memory>

class Image {
public:
    Image(const std::string& path, const std::string& base64Data);
    Image(const std::string& path);
    ~Image();

    const std::string& getPath() const;
    std::string toBase64() const;

private:
    std::string path;
    std::string base64Data;
};

class ImageList {
public:
    ImageList();
    ~ImageList();

    void addImage(std::unique_ptr<Image> image);
    std::vector<std::unique_ptr<Image>>& getImages();
    std::string generateImageTags() const;

private:
    std::vector<std::unique_ptr<Image>> images;
};
