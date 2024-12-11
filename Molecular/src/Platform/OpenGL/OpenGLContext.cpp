#include "molpch.h"
#include "Platform/OpenGL/OpenGLContext.h"

#include "Core.h"
#include "Log.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

namespace Molecular
{
    OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
        : m_windowHandle(windowHandle)
    {
        MOL_CORE_ASSERT(m_windowHandle, "Window handle is null");
    }

    void OpenGLContext::Init()
    {
        glfwMakeContextCurrent(m_windowHandle);
        int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
        MOL_CORE_ASSERT(status, "Failed to initialize OpenGL context");

        MOL_CORE_INFO("OpenGL Info:");
        MOL_CORE_INFO("     Vendor: {0}", reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
        MOL_CORE_INFO("     Renderer: {0}", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
        MOL_CORE_INFO("     Version: {0}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));
    }

    void OpenGLContext::SwapBuffers()
    {
        glfwSwapBuffers(m_windowHandle);
    }
}
