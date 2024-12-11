#pragma once

#include "Molecular/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Molecular
{
    class OpenGLContext : public GraphicsContext
    {
        public:
        OpenGLContext(GLFWwindow* windowHandle);

        void Init() override;
        void SwapBuffers() override;
        private:
        GLFWwindow* m_windowHandle;
    };
}