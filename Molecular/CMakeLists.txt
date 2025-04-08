project(Molecular)

#Define a shared library
add_library(Molecular STATIC
        src/Molecular/Physics/Atom.h
        src/Molecular/Physics/SimulationSpace.h
        src/Molecular/Physics/SimulationSpace.cpp
        src/Molecular/Physics/BoundingBox.h
        src/Molecular/Physics/Atom.cpp
)

# Add source files
file(GLOB_RECURSE MOLECULAR_SRC CONFIGURE_DEPENDS
        src/*.h
        src/*.cpp
        vendor/stb_image/*.h
        vendor/stb_image/*.cpp
)

target_sources(Molecular PRIVATE ${MOLECULAR_SRC})

# Include directories
target_include_directories(Molecular PRIVATE
        ${CMAKE_SOURCE_DIR}/Molecular/vendor/spdlog/include
        ${CMAKE_SOURCE_DIR}/Molecular/vendor/glfw/include
        ${CMAKE_SOURCE_DIR}/Molecular/vendor/glad/include
        ${CMAKE_SOURCE_DIR}/Molecular/vendor/glm/glm
        ${CMAKE_SOURCE_DIR}/Molecular/vendor/imgui
        ${CMAKE_SOURCE_DIR}/Molecular/vendor/imgui/backends
        ${CMAKE_SOURCE_DIR}/Molecular/vendor/stb_image
        ${CMAKE_SOURCE_DIR}/Molecular/src/Molecular
        ${CMAKE_SOURCE_DIR}/Molecular/src
)

target_link_libraries(Molecular PRIVATE glfw glad ImGui opengl32)

# Platform-specific settings
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

# Configuration-specific defines
target_compile_definitions(Molecular PRIVATE
        $<$<CONFIG:Debug>:MOL_DEBUG>
        $<$<CONFIG:Release>:MOL_RELEASE>
        $<$<CONFIG:Dist>:MOL_DIST>
)

# Debug symbols for Debug configuration
target_compile_options(Molecular PRIVATE
#        $<$<CONFIG:Debug>:-g>
        /utf-8
)

target_precompile_headers(Molecular PRIVATE ${CMAKE_SOURCE_DIR}/Molecular/src/molpch.h)