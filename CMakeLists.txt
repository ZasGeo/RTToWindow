# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("RTToWindow")

add_definitions(-DINTERNAL_USAGE_BUILD)
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  add_definitions(-DCOMPILER_MSVC)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX /wd4100 /wd4189 /wd4201 /GR- /EHa- /EHs- /EHc- /Oi /Od")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -opt:ref") #remove shit from exe
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -opt:ref") #removeshit from dll

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

file(COPY ${CMAKE_SOURCE_DIR}/Assets/ DESTINATION ${CMAKE_BINARY_DIR}/bin/Assets/)
#message(SEND_ERROR "foo include dire: ${CMAKE_BINARY_DIR}")
#message(SEND_ERROR "foo include dir: ${CMAKE_SOURCE_DIR}")

# Include sub-projects.
add_subdirectory ("Win32Layer")
add_subdirectory ("Engine")
add_subdirectory ("Rendering")
