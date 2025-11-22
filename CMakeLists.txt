cmake_minimum_required(VERSION 3.15)

# Project name and version
project(tsimg VERSION 0.1 LANGUAGES CXX)

find_package(Python COMPONENTS Interpreter REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Build options
option(TSIMG_STATIC_BUILD "Build with static linking" OFF)
option(TSIMG_BUILD_TESTS "Build tests" ON)

# Compiler flags
if(MSVC)
    add_compile_options(/W3 /O2)
else()
    add_compile_options(-Wall -Wextra -O3 -funroll-loops)
    if(TSIMG_STATIC_BUILD)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
    endif()
endif()

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Include directories
include_directories(include third_party)

# Build Info Generation
set(BUILD_INFO_CPP ${CMAKE_BINARY_DIR}/generated/build_info.cpp)
set(BUILD_INFO_SCRIPT ${CMAKE_SOURCE_DIR}/utils/generate_build_info.py)
set(BUILD_INFO_TXT ${CMAKE_BINARY_DIR}/build_info.txt)

add_custom_command(
    OUTPUT ${BUILD_INFO_CPP}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/generated
    COMMAND ${Python_EXECUTABLE} ${BUILD_INFO_SCRIPT}
            --output ${BUILD_INFO_CPP}
            --cmake ${CMAKE_SOURCE_DIR}/CMakeLists.txt
            --version-rc ${CMAKE_SOURCE_DIR}/version.rc
            --build-info-txt ${BUILD_INFO_TXT}
    DEPENDS 
        ${BUILD_INFO_SCRIPT}
        ${CMAKE_SOURCE_DIR}/CMakeLists.txt
    COMMENT "Generating build info"
    VERBATIM
)

# Create a library for build info to avoid recompiling everything
add_library(tsimg_build_info STATIC ${BUILD_INFO_CPP})
target_include_directories(tsimg_build_info PUBLIC ${CMAKE_SOURCE_DIR}/src)

# Main sources
set(SOURCES
    src/main.cpp
    src/tsimg_gif.cpp
    src/tsimg_spice.cpp
    src/core/TSIMGPipeline.cpp
    src/generators/SPICEGenerator.cpp
    src/generators/GIFGenerator.cpp
)

if(EXISTS ${CMAKE_SOURCE_DIR}/version.rc)
    list(APPEND SOURCES version.rc)
endif()

# Executable
add_executable(tsimg ${SOURCES})
target_link_libraries(tsimg PRIVATE tsimg_build_info)
target_include_directories(tsimg PRIVATE ${CMAKE_SOURCE_DIR}/src)

# Tests
if(TSIMG_BUILD_TESTS)
    add_executable(tsimg_test
        src/tsimg_test.cpp
        src/tsimg_test.h
    )
    target_link_libraries(tsimg_test PRIVATE tsimg_build_info)
    target_include_directories(tsimg_test PRIVATE ${CMAKE_SOURCE_DIR}/src)
endif()

# Installation
include(GNUInstallDirs)

install(TARGETS tsimg
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(EXISTS ${CMAKE_SOURCE_DIR}/templates)
    install(DIRECTORY templates DESTINATION ${CMAKE_INSTALL_DATADIR}/tsimg)
endif()

# Packaging (CPack)
set(CPACK_PACKAGE_NAME "tsimg")
set(CPACK_PACKAGE_VENDOR "NEPEM-UFSC")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Temporal Series Interactive Imager")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "tsimg")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/README.md") # Placeholder for license

if(WIN32)
    set(CPACK_GENERATOR "ZIP;NSIS")
else()
    set(CPACK_GENERATOR "TGZ;DEB")
endif()

include(CPack)
