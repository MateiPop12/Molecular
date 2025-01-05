project(Molecular)

#Define a shared library
add_library(Molecular STATIC
        src/Molecular/Core/Layer.cpp
        src/Molecular/Core/Layer.h
        src/Molecular/Core/LayerStack.cpp
        src/Molecular/Core/LayerStack.h
        src/Molecular/ImGui/ImGuiLayer.cpp
        src/Molecular/ImGui/ImGuiLayer.h
        src/Molecular/Core/Input.h
        src/Platform/Windows/WindowsInput.cpp
        src/Platform/Windows/WindowsInput.h
        src/Molecular/Core/KeyCodes.h
        src/Molecular/Core/MouseCodes.h
        src/Molecular/ImGui/ImGuiBuild.cpp
        src/Molecular/Renderer/GraphicsContext.h
        src/Platform/OpenGL/OpenGLContext.h
        src/Platform/OpenGL/OpenGLContext.cpp
        src/Molecular/Renderer/Shader.h
        src/Molecular/Renderer/Shader.cpp
        src/Molecular/Renderer/Buffer.h
        src/Molecular/Renderer/Buffer.cpp
        src/Platform/OpenGL/OpenGLBuffer.h
        src/Platform/OpenGL/OpenGLBuffer.cpp
        src/Molecular/Renderer/Renderer.h
        src/Molecular/Renderer/Renderer.cpp
        src/Molecular/Renderer/VertexArray.h
        src/Molecular/Renderer/VertexArray.cpp
        src/Platform/OpenGL/OpenGLVertexArray.h
        src/Platform/OpenGL/OpenGLVertexArray.cpp
        src/Molecular/Renderer/RendererAPI.h
        src/Molecular/Renderer/rENDERERapi.cpp
        src/Molecular/Renderer/RenderCommand.h
        src/Molecular/Renderer/RenderCommand.cpp
        src/Platform/OpenGL/OpenGLRendererAPI.h
        src/Platform/OpenGL/OpenGLRendererAPI.cpp
        src/Molecular/Renderer/OrthographicCamera.h
        src/Molecular/Renderer/OrthographicCamera.cpp
        src/Molecular/Core/Timestep.h
        src/Platform/OpenGL/OpenGLShader.h
        src/Platform/OpenGL/OpenGLShader.cpp
        src/Molecular/Renderer/Texture.h
        src/Molecular/Renderer/Texture.cpp
        src/Platform/OpenGL/OpenGLTexture.h
        src/Platform/OpenGL/OpenGLTexture.cpp
        vendor/stb_image/stb_image.h
        vendor/stb_image/stb_image.cpp
        src/Molecular/Renderer/OrthographicCameraController.h
        src/Molecular/Renderer/OrthographicCameraController.cpp
)

# Add source files
file(GLOB_RECURSE MOLECULAR_SRC CONFIGURE_DEPENDS
        src/*.h
        src/*.cpp
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