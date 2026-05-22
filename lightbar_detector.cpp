#include "lightbar_detector.hpp"
#include <opencv2/imgproc.hpp>

// 红色在 HSV 中跨越 0° 两侧，分两段阈值
static const cv::Scalar RED_LOW1(0, 100, 100);
static const cv::Scalar RED_HIGH1(10, 255, 255);
static const cv::Scalar RED_LOW2(160, 100, 100);
static const cv::Scalar RED_HIGH2(180, 255, 255);
// 蓝色 HSV 阈值
static const cv::Scalar BLUE_LOW(100, 100, 100);
static const cv::Scalar BLUE_HIGH(130, 255, 255);

cv::Mat thresholdRedBlue(const cv::Mat& src) {
    cv::Mat hsv;
    cv::cvtColor(src, hsv, cv::COLOR_BGR2HSV);
    // cv::cvtColor: 颜色空间转换 BGR -> HSV，便于颜色阈值分割

    // 提取红色（两段合并）
    cv::Mat redMask1, redMask2, redMask;
    cv::inRange(hsv, RED_LOW1, RED_HIGH1, redMask1);
    // cv::inRange: 提取 HSV 中 [RED_LOW1, RED_HIGH1] 范围内的像素，输出二值掩码
    cv::inRange(hsv, RED_LOW2, RED_HIGH2, redMask2);
    // cv::inRange: 提取红色第二段（H 在 160~180 之间）
    cv::bitwise_or(redMask1, redMask2, redMask);
    // cv::bitwise_or: 按位或合并两段红色掩码

    // 提取蓝色
    cv::Mat blueMask;
    cv::inRange(hsv, BLUE_LOW, BLUE_HIGH, blueMask);
    // cv::inRange: 提取蓝色 HSV 范围内的像素

    // 合并红蓝
    cv::Mat combined;
    cv::bitwise_or(redMask, blueMask, combined);
    // cv::bitwise_or: 合并红蓝掩码
    return combined;
}

std::vector<std::vector<cv::Point>> extractContours(const cv::Mat& mask) {
    std::vector<std::vector<cv::Point>> contours;
    cv::Mat maskCopy = mask.clone();
    cv::findContours(maskCopy, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    // cv::findContours: 查找二值图像轮廓，RETR_EXTERNAL 仅取最外层，CHAIN_APPROX_SIMPLE 压缩冗余顶点
    return contours;
}

cv::Mat applyMaskToImage(const cv::Mat& src, const cv::Mat& mask) {
    cv::Mat result;
    cv::bitwise_and(src, src, result, mask);
    // cv::bitwise_and: 用掩码提取原图中对应区域，其余置黑
    return result;
}

cv::Mat drawAllContours(const cv::Mat& src,
                        const std::vector<std::vector<cv::Point>>& contours) {
    cv::Mat out = src.clone();
    cv::drawContours(out, contours, -1, cv::Scalar(0, 255, 0), 2);
    // cv::drawContours: 绘制全部轮廓，绿色(0,255,0)，线宽 2px
    return out;
}

std::vector<std::vector<cv::Point>> filterLightBars(
    const std::vector<std::vector<cv::Point>>& contours,
    double minAspectRatio, double minArea) {
    std::vector<std::vector<cv::Point>> result;
    for (const auto& c : contours) {
        double area = cv::contourArea(c);
        // cv::contourArea: 计算轮廓面积，用于过滤噪点
        if (area < minArea) continue;

        cv::RotatedRect rect = cv::minAreaRect(c);
        // cv::minAreaRect: 计算轮廓的最小面积外接旋转矩形
        float w = rect.size.width;
        float h = rect.size.height;
        if (w < 1.0f || h < 1.0f) continue;

        float aspect = std::max(w, h) / std::min(w, h);
        // 长宽比 = 长边 / 短边，RoboMaster 灯条细长（通常 > 2.5）
        if (aspect >= minAspectRatio) {
            result.push_back(c);
        }
    }
    return result;
}

cv::Mat drawLightBarRects(const cv::Mat& src,
                          const std::vector<std::vector<cv::Point>>& lightBars) {
    cv::Mat out = src.clone();
    for (const auto& c : lightBars) {
        cv::RotatedRect rect = cv::minAreaRect(c);
        // cv::minAreaRect: 获取灯条轮廓的最小面积外接旋转矩形
        cv::Point2f vertices[4];
        rect.points(vertices);
        // RotatedRect::points: 获取旋转矩形的四个顶点
        for (int j = 0; j < 4; j++) {
            cv::line(out, vertices[j], vertices[(j + 1) % 4],
                     cv::Scalar(0, 255, 255), 2);
            // cv::line: 绘制矩形边，黄色(0,255,255)，线宽 2px
        }
    }
    return out;
}
