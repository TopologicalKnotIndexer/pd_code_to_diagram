# 指定CMake最低版本
cmake_minimum_required(VERSION 3.10)

# 项目名称（可自定义，支持指定语言：CXX=C++，C=C，All=两者）
project(pdcode_to_diagram LANGUAGES CXX)

# 设置编译选项
set(CMAKE_CXX_STANDARD 11)          # 指定C++标准
set(CMAKE_CXX_STANDARD_REQUIRED ON) # 强制要求指定的C++标准

# 添加可执行文件
add_executable(pdcode_to_diagram src/main.cpp)
