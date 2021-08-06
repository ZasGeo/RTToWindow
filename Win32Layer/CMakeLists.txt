# CMakeList.txt : CMake project for RTToWindow, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    add_definitions(-DUNICODE -D_UNICODE)
    add_executable(RTToWindow WIN32 "Win32Layer.cpp") #WIN32 So the console window does not open on Windows
    target_link_libraries(RTToWindow Xinput.lib)
    target_include_directories(RTToWindow PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../)
    #target_link_libraries(RTToWindow Engine)
endif()