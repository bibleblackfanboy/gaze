cmake_minimum_required(VERSION 3.15)

project(dream_curio)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(libs/fltk)

add_executable(dream_curio WIN32 
    src/main.cpp

    src/logger.hpp
    src/logger.cpp

    src/State.hpp

    src/BaseWindow.hpp
    src/BaseWindow.cpp

    src/MarkerLogic.hpp
    src/MarkerLogic.cpp
    src/MarkerTiming.hpp
    src/MarkerTiming.cpp
    src/MarkerWindow.hpp
    src/MarkerWindow.cpp

    src/CalibrationLogic.hpp
    src/CalibrationLogic.cpp
    src/CalibrationWindow.hpp
    src/CalibrationWindow.cpp

    src/TrackingLogic.hpp
    src/TrackingLogic.cpp
    src/TrackingWindow.hpp
    src/TrackingWindow.cpp
)

target_link_libraries(dream_curio PRIVATE fltk)

