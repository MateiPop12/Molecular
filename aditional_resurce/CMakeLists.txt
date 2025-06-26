cmake_minimum_required(VERSION 3.24)

# Define the project
project(ImGui LANGUAGES CXX)

# Add the library target
add_library(ImGui STATIC
        "imconfig.h"
        "imgui.h"
        "imgui.cpp"
        "imgui_draw.cpp"
        "imgui_internal.h"
        "imgui_widgets.cpp"
        "imstb_rectpack.h"
        "imstb_textedit.h"
        "imstb_truetype.h"
        "imgui_demo.cpp"
        "imgui_tables.cpp"
        "imgui_widgets.cpp"
)

# Set the target properties for output directories
set_target_properties(ImGui PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}/${PROJECT_NAME}"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}/${PROJECT_NAME}"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}/${PROJECT_NAME}"
)

# Platform-specific settings
if(WIN32)
    target_compile_features(ImGui PUBLIC cxx_std_17)
    target_compile_definitions(ImGui PRIVATE WINDOWS)
    set_target_properties(ImGui PROPERTIES
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
    )
elseif(UNIX AND NOT APPLE)
    target_compile_features(ImGui PUBLIC cxx_std_17)
    target_compile_options(ImGui PRIVATE -fPIC)
endif()

# Configuration-specific settings
target_compile_options(ImGui PRIVATE
        $<$<CONFIG:Debug>:-g>    # Debug symbols
        $<$<CONFIG:Release>:-O3> # Optimization for Release
)