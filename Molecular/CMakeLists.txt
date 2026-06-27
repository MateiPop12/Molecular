project(Molecular)

add_library(Molecular STATIC)

# Source files — stb_image is included here via GLOB, so it must NOT
# also be added as add_subdirectory() in the root CMakeLists.txt.
file(GLOB_RECURSE MOLECULAR_SRC CONFIGURE_DEPENDS
        src/*.h
        src/*.cpp
        vendor/stb_image/*.h
        vendor/stb_image/*.cpp
)

target_sources(Molecular PRIVATE ${MOLECULAR_SRC})

# PUBLIC include dirs are inherited by any target that links Molecular,
# so Sandbox no longer needs to repeat them manually.
target_include_directories(Molecular
        PUBLIC
        ${CMAKE_SOURCE_DIR}/Molecular/src
        ${CMAKE_SOURCE_DIR}/Molecular/src/Molecular
        ${CMAKE_SOURCE_DIR}/Molecular/vendor/spdlog/include
        ${CMAKE_SOURCE_DIR}/Molecular/vendor/glm/glm
        ${CMAKE_SOURCE_DIR}/Molecular/vendor/imgui
        ${CMAKE_SOURCE_DIR}/Molecular/vendor/imgui/backends
        PRIVATE
        ${CMAKE_SOURCE_DIR}/Molecular/vendor/glfw/include
        ${CMAKE_SOURCE_DIR}/Molecular/vendor/glad/include
        ${CMAKE_SOURCE_DIR}/Molecular/vendor/stb_image
)

target_link_libraries(Molecular PRIVATE glfw glad ImGui opengl32)

if(WIN32)
    target_compile_definitions(Molecular PRIVATE
            MOL_PLATFORM_WINDOWS
            MOL_BUILD_DLL
            GLFW_INCLUDE_NONE
    )
    set_target_properties(Molecular PROPERTIES
            CXX_STANDARD 17
            CXX_STANDARD_REQUIRED ON
    )
endif()

target_compile_definitions(Molecular PRIVATE
        $<$<CONFIG:Debug>:MOL_DEBUG>
        $<$<CONFIG:Release>:MOL_RELEASE>
        $<$<CONFIG:Dist>:MOL_DIST>
)

# Absolute path to the runtime assets directory, baked in at configure time so
# the executable locates shaders/textures regardless of the working directory.
# PUBLIC so Sandbox (and anything else linking Molecular) inherits it too.
target_compile_definitions(Molecular PUBLIC
        MOL_ASSETS_DIR="${CMAKE_SOURCE_DIR}/Sandbox/assets"
)

target_compile_options(Molecular PRIVATE /utf-8)

target_precompile_headers(Molecular PRIVATE ${CMAKE_SOURCE_DIR}/Molecular/src/molpch.h)