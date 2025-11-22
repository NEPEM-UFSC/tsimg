cmake_minimum_required(VERSION 3.15)

# Project name and version
file(READ "VERSION" VER_RAW)
string(STRIP "${VER_RAW}" PROJECT_VERSION_STRING)
project(tsimg VERSION ${PROJECT_VERSION_STRING} LANGUAGES CXX)

find_package(Python COMPONENTS Interpreter REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Build options
option(TSIMG_STATIC_BUILD "Build with static linking" OFF)
option(TSIMG_BUILD_TESTS "Build tests" ON)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Build Info Generation
set(BUILD_INFO_CPP ${CMAKE_BINARY_DIR}/generated/build_info.cpp)
set(BUILD_INFO_SCRIPT ${CMAKE_SOURCE_DIR}/utils/generate_build_info.py)
set(BUILD_INFO_TXT ${CMAKE_SOURCE_DIR}/utils/build_info.txt)

add_custom_command(
    OUTPUT ${BUILD_INFO_CPP}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/generated
    COMMAND ${Python_EXECUTABLE} ${BUILD_INFO_SCRIPT}
            --output ${BUILD_INFO_CPP}
            --project-version ${PROJECT_VERSION}
            --version-rc ${CMAKE_SOURCE_DIR}/version.rc
            --build-info-txt ${BUILD_INFO_TXT}
    DEPENDS 
        ${BUILD_INFO_SCRIPT}
        ${CMAKE_SOURCE_DIR}/VERSION
    COMMENT "Generating build info"
    VERBATIM
)

# Create a library for build info
add_library(tsimg_build_info STATIC ${BUILD_INFO_CPP})
target_include_directories(tsimg_build_info PUBLIC ${CMAKE_SOURCE_DIR}/src)

# Core Library Sources
set(CORE_SOURCES
    src/tsimg_gif.cpp
    src/core/TSIMGPipeline.cpp
    src/generators/SPICEGenerator.cpp
    src/generators/GIFGenerator.cpp
    
    # Utils
    src/utils/Logger.cpp
    src/utils/FileHandler.cpp
    src/utils/ImageValidator.cpp
    src/utils/Base64.cpp
    src/utils/FileIO.cpp
    src/utils/JsonUtils.cpp
    src/utils/ImageProcessor.cpp
    
    # Spice Core
    src/spice/Image.cpp
    src/spice/SpiceContent.cpp
    src/spice/HTMLBuilder.cpp
    src/spice/TemplateWriter.cpp
    src/spice/SPICEBuilder.cpp
    src/spice/SPICE.cpp
)

# Core Library
add_library(tsimg_core STATIC ${CORE_SOURCES})
target_include_directories(tsimg_core PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/third_party>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(tsimg_core PUBLIC tsimg_build_info)

# Compiler Options for Core
if(MSVC)
    target_compile_options(tsimg_core PRIVATE /W3 /O2)
else()
    target_compile_options(tsimg_core PRIVATE -Wall -Wextra -O3 -funroll-loops)
    if(TSIMG_STATIC_BUILD)
        target_link_options(tsimg_core PUBLIC -static-libgcc -static-libstdc++ -static)
    endif()
endif()

# Main Executable
set(MAIN_SOURCES src/main.cpp)
if(EXISTS ${CMAKE_SOURCE_DIR}/version.rc)
    list(APPEND MAIN_SOURCES version.rc)
endif()

add_executable(tsimg ${MAIN_SOURCES})
target_link_libraries(tsimg PRIVATE tsimg_core)

# Tests
if(TSIMG_BUILD_TESTS)
    add_executable(tsimg_test
        src/tsimg_test.cpp
        src/tsimg_test.h
    )
    target_link_libraries(tsimg_test PRIVATE tsimg_core)
endif()

# Installation
include(GNUInstallDirs)

install(TARGETS tsimg tsimg_core
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

if(EXISTS ${CMAKE_SOURCE_DIR}/templates)
    install(DIRECTORY templates DESTINATION ${CMAKE_INSTALL_DATADIR}/tsimg)
endif()

# Packaging (CPack)
set(CPACK_PACKAGE_NAME "tsimg")
set(CPACK_PACKAGE_VENDOR "NEPEM-UFSC")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Temporal Series Interactive Imager")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/NEPEM-UFSC/tsimg")
set(CPACK_PACKAGE_CONTACT "NEPEM-UFSC")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "tsimg")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/README.md")

# Install documentation
install(FILES README.md DESTINATION ${CMAKE_INSTALL_DOCDIR})

if(WIN32)
    set(CPACK_GENERATOR "ZIP;NSIS")
    set(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_NAME} ${CPACK_PACKAGE_VERSION}")
    set(CPACK_NSIS_CONTACT "${CPACK_PACKAGE_CONTACT}")
    set(CPACK_NSIS_MODIFY_PATH ON)
else()
    set(CPACK_GENERATOR "TGZ;DEB")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${CPACK_PACKAGE_CONTACT}")
endif()

include(CPack)
