cmake_minimum_required(VERSION 3.14)

project(
    bmplib
    VERSION 0.0.1
    DESCRIPTION "A C++ library for working with computer graphics"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_STANDARD_REQUIRED ON)


# ---- Declare library ---
add_library(
	bmplib STATIC
	src/Image.cpp
    src/Graphics.cpp
	src/Algorithms.cpp
)

#публично объявляем директорию заголовков
target_include_directories(
    bmplib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

#включаем строгие предупреждения компилятора
target_compile_options(bmplib PRIVATE -Wall -Wextra -Wpedantic)