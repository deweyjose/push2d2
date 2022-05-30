cmake_minimum_required(VERSION 3.7)

set(CMAKE_VERBOSE_MAKEFILE ON)

project(P2D2 VERSION 0.1)

include_directories(src/include)

add_subdirectory(src)
add_subdirectory(test)

enable_testing()
