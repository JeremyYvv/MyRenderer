cmake_minimum_required (VERSION 3.8)

project (MyRenderer)

add_compile_options(-g -O0)

include_directories(./)

add_executable (MyRenderer "main.cpp" "tgaImage.cpp" "model.cpp")

