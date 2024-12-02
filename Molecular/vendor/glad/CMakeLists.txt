cmake_minimum_required(VERSION 3.24)

# Define the project
project(glad LANGUAGES C)

# Add the library target
add_library(glad STATIC
        "include/glad/glad.h"
        "include/KHR/khrplatform.h"
        "src/glad.c"
)

# Set the target properties for output directories
set_target_properties(glad PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}/${PROJECT_NAME}"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}/${PROJECT_NAME}"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}/${PROJECT_NAME}"
)

# Include directories
target_include_directories(glad PUBLIC "include")

# Handle specific configurations and system settings
if(WIN32)
    target_compile_definitions(glad PRIVATE WINDOWS)
endif()

# Set configurations
target_compile_options(glad PRIVATE
        $<$<CONFIG:Debug>:-g>   # Debug symbols
        $<$<CONFIG:Release>:-O3>  # Optimization for Release
)

# Set runtime library
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
