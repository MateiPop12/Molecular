#include "Molecular/Renderer/RenderCommand.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Molecular
{
    RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI();
}
