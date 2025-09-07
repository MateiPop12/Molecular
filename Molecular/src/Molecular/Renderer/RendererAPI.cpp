#include "RendererAPI.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Molecular
{
    RendererAPI::API RendererAPI::s_API = API::OpenGL;

    Scope<RendererAPI> RendererAPI::Create()
    {
        switch (s_API)
        {
        case API::None:    MOL_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
        case API::OpenGL:  return CreateScope<OpenGLRendererAPI>();
        }

        MOL_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }
}