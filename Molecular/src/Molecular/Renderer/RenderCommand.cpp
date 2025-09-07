#include "Molecular/Renderer/RenderCommand.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Molecular
{
    Scope<RendererAPI> RenderCommand::s_RendererAPI = RendererAPI::Create();
}
