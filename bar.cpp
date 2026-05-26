#include "io_utils.hpp"
#include "lightbar_detector.hpp"
#include <iostream>
#include <filesystem>

int main() {
    std::string imgDir = "/home/xxx/python/demo/images";
    std::string resultsDir = "/home/xxx/python/demo/build1/results";

    // 确保输出目录存在
    if (!std::filesystem::exists(resultsDir)) {
        std::filesystem::create_directories(resultsDir);
        // fs::create_directories: 递归创建 results 目录
    }

    // 扫描输入图片
    auto images = getImagePaths(imgDir);
    if (images.empty()) {
        std::cerr << "No images found in " << imgDir << std::endl;
        return 1;
    }
    std::cout << "Found " << images.size() << " image(s). Processing..." << std::endl;

    for (const auto& path : images) {
        std::string stem = path.stem().string();

        // 读取原图
        cv::Mat src = readImage(path);  //BGR   RGB RAW
        if (src.empty()) continue;
        
        cv::addWeighted(src, 0.6, src, 0.0, 0.0, src);  //i^0.6
        // 红色阈值分割
        cv::Mat mask = thresholdRed(src);

        // 3. 保存阈值处理结果
        saveImage(resultsDir + "/" + stem + "_threshold.png", applyMaskToImage(src, mask));

        // 4. 提取轮廓
        auto contours = extractContours(mask);

        // 5. 保存轮廓图
        saveImage(resultsDir + "/" + stem + "_contours.png", drawAllContours(src, contours));

        // 6. 筛选灯条并按最小外接矩形绘制
        auto lightBars = filterLightBars(contours);

        // 7. 保存最终结果图
        saveImage(resultsDir + "/" + stem + "_result.png", drawLightBarRects(src, lightBars));

        std::cout << "Processed: " << stem << std::endl;
    }

    std::cout << "Done. Results saved to " << resultsDir << std::endl;
    return 0;
}
