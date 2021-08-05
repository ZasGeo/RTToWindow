cmake_minimum_required (VERSION 3.8)

add_library(Engine SHARED "Engine.cpp")
target_link_options(Engine PUBLIC "/EXPORT:UpdateAndRender")
target_link_options(Engine PUBLIC "-PDB:${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Engine_%random%.pdb")