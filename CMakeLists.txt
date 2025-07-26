cmake_minimum_required(VERSION 3.10)
project(muduo-core)

# muduo-core最终编译成so动态库，设置动态库的路径
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)

# 设置调试信息
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++17")

# 定义参与编译的源代码文件
aux_source_directory(. SRC_LIST)

# 编译生成动态库
add_library(muduo-core SHARED ${SRC_LIST})

