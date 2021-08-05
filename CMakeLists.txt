# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("RTToWindow")

add_definitions(-DINTERNAL_USAGE_BUILD)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX /wd4100 /GR- /EHa- /EHs- /EHc- /Oi /Od")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -opt:ref") #remove shit from exe
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -opt:ref") #removeshit from dll

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
# Include sub-projects.
add_subdirectory ("Win32Layer")
add_subdirectory ("Engine")