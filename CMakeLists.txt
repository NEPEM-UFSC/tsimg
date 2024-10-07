cmake_minimum_required(VERSION 3.10)

# Project name and version
project(tsimg VERSION 0.1)

# Set the C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -funroll-loops")
set(CMAKE_BUILD_TYPE Release)

# Include directories
include_directories(include third_party)

# Add source files
set(SOURCES
    src/main.cpp
    # src/gif_writer.cpp
    src/spice_constructor.cpp
)

# Create an executable
add_executable(tsimg ${SOURCES})

# Link libraries (none for now, add later if necessary)
