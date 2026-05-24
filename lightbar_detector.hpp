#pragma once

#include <opencv2/core.hpp>
#include <vector>

// 对原图做红蓝颜色阈值分割，返回二值掩码
cv::Mat thresholdRedBlue(const cv::Mat& src);

// 从掩码中提取轮廓（最外层）
std::vector<std::vector<cv::Point>> extractContours(const cv::Mat& mask);

// 将掩码与原图叠加，得到阈值处理可视化结果
cv::Mat applyMaskToImage(const cv::Mat& src, const cv::Mat& mask);

// 在原图上绘制所有轮廓
cv::Mat drawAllContours(const cv::Mat& src,
                        const std::vector<std::vector<cv::Point>>& contours);

// 按灯条形状（长宽比）过滤轮廓
std::vector<std::vector<cv::Point>> filterLightBars(
    const std::vector<std::vector<cv::Point>>& contours,
    double minAspectRatio = 3,
    double minArea = 50.0);

// 在原图上绘制灯条的最小外接旋转矩形
cv::Mat drawLightBarRects(const cv::Mat& src,
                          const std::vector<std::vector<cv::Point>>& lightBars);
