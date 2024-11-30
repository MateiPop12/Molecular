project(Sandbox)

# Define an executable
add_executable(Sandbox)

# Add source files
file(GLOB_RECURSE SANDBOX_SRC CONFIGURE_DEPENDS
        src/*.h
        src/*.cpp
)

target_sources(Sandbox PRIVATE ${SANDBOX_SRC})

# Include directories
target_include_directories(Sandbox PRIVATE
        ${CMAKE_SOURCE_DIR}/Molecular/src
        ${CMAKE_SOURCE_DIR}/Molecular/vendor/spdlog/include
)

# Link Molecular library
target_link_libraries(Sandbox PRIVATE Molecular)

# Platform-specific settings
if(WIN32)
    target_compile_definitions(Sandbox PRIVATE MOL_PLATFORM_WINDOWS)
    set_target_properties(Sandbox PROPERTIES
            CXX_STANDARD 17
            CXX_STANDARD_REQUIRED ON
    )
endif()

# Configuration-specific defines
target_compile_definitions(Sandbox PRIVATE
        $<$<CONFIG:Debug>:MOL_DEBUG>
        $<$<CONFIG:Release>:MOL_RELEASE>
        $<$<CONFIG:Dist>:MOL_DIST>
)

# Debug symbols for Debug configuration
target_compile_options(Sandbox PRIVATE
#        $<$<CONFIG:Debug>:-g>
        /utf-8
)
