cmake_minimum_required(VERSION 3.15)

project(mosVision)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(libs/fltk)

add_executable(mosVision WIN32 
    src/main.cpp
)

target_link_libraries(mosVision PRIVATE fltk)

