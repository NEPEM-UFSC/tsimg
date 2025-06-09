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
    COMMAND ${CMAKE_COMMAND} -E echo "Generating build info"
    COMMAND ${CMAKE_COMMAND} -E env python ${CMAKE_SOURCE_DIR}/utils/generate_build_info.py
    DEPENDS 
        ${CMAKE_SOURCE_DIR}/utils/generate_build_info.py 
        ${CMAKE_SOURCE_DIR}/src/main.cpp 
        ${CMAKE_SOURCE_DIR}/src/tsimg_gif.cpp 
        ${CMAKE_SOURCE_DIR}/src/tsimg_spice.cpp
        ${CMAKE_SOURCE_DIR}/CMakeLists.txt
    COMMENT "Generating build info"
    VERBATIM
)

# Add a custom target to always run the build info generation
add_custom_target(generate_build_info ALL
    DEPENDS ${CMAKE_SOURCE_DIR}/src/build_info.h
)

# Force regeneration by removing the output file before each build
add_custom_target(clean_build_info
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_SOURCE_DIR}/src/build_info.h
    COMMENT "Cleaning build info for regeneration"
)

add_dependencies(generate_build_info clean_build_info)

# Definição das fontes principais do projeto
set(SOURCES
    src/build_info.h
    src/main.cpp
    src/tsimg_gif.cpp
    src/tsimg_spice.cpp
)

# Adicionar version.rc apenas se existir
if(EXISTS ${CMAKE_SOURCE_DIR}/version.rc)
    list(APPEND SOURCES version.rc)
    message(STATUS "Found version.rc, adding to sources")
else()
    message(STATUS "version.rc not found, skipping")
endif()

# Define the executable
add_executable(tsimg ${SOURCES})

# Ensure the build info is generated before compiling the executable
add_dependencies(tsimg generate_build_info)

set_target_properties(tsimg PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

# Executável de teste - usar implementação mock em vez das originais
add_executable(tsimg_test
    src/tsimg_test.cpp
    src/tsimg_test.h
    src/build_info.h
)

# Garantir que as informações de build sejam geradas antes de compilar os testes
add_dependencies(tsimg_test generate_build_info)

# Definir propriedades do executável de teste
set_target_properties(tsimg_test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)