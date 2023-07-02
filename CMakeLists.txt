cmake_minimum_required(VERSION 3.12)

project(labwork12 VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDART 20)

add_subdirectory(bin)
add_subdirectory(lib)
add_subdirectory(tests)

enable_testing()