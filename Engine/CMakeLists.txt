cmake_minimum_required (VERSION 3.8)

add_library(Engine SHARED "Engine.cpp" "Allocators.cpp" "Entity.cpp" "Bitmap.cpp")
target_link_options(Engine PUBLIC "/EXPORT:UpdateAndRender")
target_link_options(Engine PUBLIC "/EXPORT:Initialize")
target_link_options(Engine PUBLIC "-PDB:${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Engine_%random%.pdb")
target_link_libraries(Engine Rendering)

target_include_directories(Engine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../)