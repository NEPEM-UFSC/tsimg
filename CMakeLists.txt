cmake_minimum_required(VERSION 3.10)

# Project name and version
project(tsimg VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -funroll-loops")
set(CMAKE_BUILD_TYPE Debug)
# set(CMAKE_BUILD_TYPE Release)

include_directories(include third_party)

# Add a custom command to generate build info
add_custom_command(
    OUTPUT ${CMAKE_SOURCE_DIR}/src/build_info.h
    COMMAND ${CMAKE_COMMAND} -E env python ${CMAKE_SOURCE_DIR}/utils/generate_build_info.py
    DEPENDS ${CMAKE_SOURCE_DIR}/utils/generate_build_info.py
)

# Ensure the build info header is generated before building the target
add_custom_target(generate_build_info DEPENDS ${CMAKE_SOURCE_DIR}/src/build_info.h)

set(SOURCES
    src/main.cpp
    src/tsimg_gif.cpp
    src/tsimg_spice.cpp
    version.rc
)

add_executable(tsimg ${SOURCES})

# Make sure the build info is generated before building the executable
add_dependencies(tsimg generate_build_info)
