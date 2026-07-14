cmake_minimum_required(VERSION 3.15)

project(dream_curio)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(libs/fltk)

add_executable(dream_curio WIN32 
    src/main.cpp
    src/logger.cpp
    src/logger.hpp
)

target_link_libraries(dream_curio PRIVATE fltk)

