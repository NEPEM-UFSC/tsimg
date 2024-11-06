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

set(SOURCES
    src/main.cpp
    src/tsimg.cpp
    src/spice_constructor.cpp
    src/image_utils.cpp
    src/template_writer.cpp
    version.rc
)

add_executable(tsimg ${SOURCES})

add_custom_command(TARGET tsimg POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_SOURCE_DIR}/tsimg.manifest $<TARGET_FILE_DIR:tsimg>/tsimg.manifest
)

# Vincula o manifesto ao executável
if (WIN32)
    add_custom_command(TARGET tsimg POST_BUILD
        COMMAND "C:/Program Files (x86)/Windows Kits/10/bin/10.0.26100.0/x64/mt.exe" -manifest $<TARGET_FILE_DIR:tsimg>/tsimg.manifest -outputresource:$<TARGET_FILE:tsimg>;#1
        COMMENT "Embedding manifest into executable"
    )
endif()