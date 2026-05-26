#include "lightbar_detector.hpp"
#include <opencv2/imgproc.hpp>

cv::Mat thresholdRed(const cv::Mat& src) {
    // 在 BGR 空间直接提取红色：R 通道显著高于 G 和 B，单次判断无需拼接
    cv::Mat channels[3];
    cv::split(src, channels);
    cv::Mat redMask = (channels[2] > channels[1] * 1.2)
                    & (channels[2] > channels[0] * 1.2)
                    & (channels[2] > 135);
    return redMask;
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
        float w = rect.size.width;
        float h = rect.size.height;
        float angle = std::abs(rect.angle);
        // 长边与水平面夹角：width >= height 时夹角即 |angle|，否则 height 边垂直于 width 边
        float longEdgeAngle = (w >= h) ? angle : (90.0f - angle);
        if (longEdgeAngle < 75.0f) continue;

        cv::Point2f vertices[4];
        rect.points(vertices);
        for (int j = 0; j < 4; j++) {
            cv::line(out, vertices[j], vertices[(j + 1) % 4],
                     cv::Scalar(0, 255, 255), 2);
        }
    }
    return out;
}
