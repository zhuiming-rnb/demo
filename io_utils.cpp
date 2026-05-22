#include "io_utils.hpp"
#include <opencv2/imgcodecs.hpp>
#include <iostream>
#include <algorithm>

namespace fs = std::filesystem;

std::vector<fs::path> getImagePaths(const std::string& dir) {
    std::vector<fs::path> paths;
    for (const auto& entry : fs::directory_iterator(dir)) {
        if (!entry.is_regular_file()) continue;
        auto ext = entry.path().extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        if (ext == ".jpg" || ext == ".jpeg" || ext == ".png" || ext == ".bmp") {
            paths.push_back(entry.path());
        }
    }
    return paths;
}

cv::Mat readImage(const fs::path& path) {
    cv::Mat img = cv::imread(path.string());
    // cv::imread: 从文件读取图像数据
    if (img.empty()) {
        std::cerr << "Failed to read: " << path << std::endl;
    }
    return img;
}

void saveImage(const std::string& path, const cv::Mat& image) {
    cv::imwrite(path, image);
    // cv::imwrite: 将图像写入文件
}
