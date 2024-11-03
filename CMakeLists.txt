cmake_minimum_required(VERSION 3.10)

# Project name and version
project(tsimg VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -funroll-loops")
# set(CMAKE_BUILD_TYPE Release)

include_directories(include third_party)

set(SOURCES
    src/main.cpp
    src/tsimg.cpp
    src/spice_constructor.cpp
    src/image_utils.cpp
)

add_executable(tsimg ${SOURCES})