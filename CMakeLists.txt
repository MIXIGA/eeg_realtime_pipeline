cmake_minimum_required(VERSION 3.10)
project(eeg_pipeline)

# 设置C++标准
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 头文件搜索路径
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/third_party/kissfft)

# 收集源文件
file(GLOB_RECURSE SOURCES
    ${PROJECT_SOURCE_DIR}/src/*.cpp
    ${PROJECT_SOURCE_DIR}/third_party/kissfft/kiss_fft.c
)

# 生成可执行文件
add_executable(eeg_pipeline ${SOURCES})

# 链接pthread库
target_link_libraries(eeg_pipeline pthread)

# 开启编译优化（可选，提升性能）
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
