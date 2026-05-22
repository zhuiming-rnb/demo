#pragma once

#include <opencv2/core.hpp>
#include <string>
#include <vector>
#include <filesystem>

// 读取指定目录下所有图片路径（支持 jpg, jpeg, png, bmp）
std::vector<std::filesystem::path> getImagePaths(const std::string& dir);

// 读取图片，失败返回空 Mat
cv::Mat readImage(const std::filesystem::path& path);

// 保存图片到指定路径
void saveImage(const std::string& path, const cv::Mat& image);
