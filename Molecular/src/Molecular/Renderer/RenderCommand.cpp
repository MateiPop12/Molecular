#include "Molecular/Renderer/RenderCommand.h"
#include "Platform/OpenGL/OpenGLRenderer.h"

namespace Molecular
{
    RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRenderer();
}
