cmake_minimum_required(VERSION 3.14)
project(bar LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找 OpenCV
find_package(OpenCV REQUIRED)

# 可执行文件
add_executable(bar
    bar.cpp
    io_utils.cpp
    lightbar_detector.cpp
)

# 链接 OpenCV
target_link_libraries(bar PRIVATE ${OpenCV_LIBS})

# 头文件目录
target_include_directories(bar PRIVATE ${CMAKE_SOURCE_DIR} ${OpenCV_INCLUDE_DIRS})
