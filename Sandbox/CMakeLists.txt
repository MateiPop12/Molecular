project(Sandbox)

add_executable(Sandbox)

file(GLOB_RECURSE SANDBOX_SRC CONFIGURE_DEPENDS
        src/*.h
        src/*.cpp
)

target_sources(Sandbox PRIVATE ${SANDBOX_SRC})

# No need to repeat Molecular's include dirs here — they are inherited
# transitively because Molecular exposes them as PUBLIC.
target_link_libraries(Sandbox PRIVATE Molecular)

if(WIN32)
    target_compile_definitions(Sandbox PRIVATE MOL_PLATFORM_WINDOWS)
    set_target_properties(Sandbox PROPERTIES
            CXX_STANDARD 17
            CXX_STANDARD_REQUIRED ON
    )
endif()

target_compile_definitions(Sandbox PRIVATE
        $<$<CONFIG:Debug>:MOL_DEBUG>
        $<$<CONFIG:Release>:MOL_RELEASE>
        $<$<CONFIG:Dist>:MOL_DIST>
)

target_compile_options(Sandbox PRIVATE /utf-8)