cmake_minimum_required(VERSION 3.24)
project(MolecularWorkspace)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enforce a consistent MSVC runtime across all targets to avoid LNK4098
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDebugDLL" CACHE STRING "" FORCE)

# Output directories
set(OUTPUT_DIR ${CMAKE_BUILD_TYPE}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${OUTPUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${OUTPUT_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin-int/${OUTPUT_DIR})

# Vendor libraries
add_subdirectory(Molecular/vendor/glfw)
add_subdirectory(Molecular/vendor/glad)
add_subdirectory(Molecular/vendor/imgui)
# stb_image is header-only / single compilation unit — compiled directly
# into Molecular via GLOB, so it must NOT also be a separate subdirectory.

enable_testing()

# Project targets
add_subdirectory(Molecular)
add_subdirectory(Sandbox)
add_subdirectory(tests)