# CMakeList.txt: TinyRenderer 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

project (MyRenderer)

add_compile_options(-g -O0)

include_directories(./)

# 将源代码添加到此项目的可执行文件。
add_executable (MyRenderer "main.cpp" "tgaImage.cpp" "model.cpp")

