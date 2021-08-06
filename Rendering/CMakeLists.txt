cmake_minimum_required (VERSION 3.8)

add_library(Rendering "SimpleRenders.cpp" "SimpleRenders.h")
target_include_directories(Rendering PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../)