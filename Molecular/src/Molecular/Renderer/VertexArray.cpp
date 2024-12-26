#include "VertexArray.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace Molecular
{
    VertexArray::~VertexArray()
    {
    }

    VertexArray* VertexArray::Create()
    {
        switch (Renderer::GetAPI())
        {
        case RendererAPI::API::None      :MOL_CORE_ASSERT(false,"RendererAPI::None not supported"); return nullptr;
        case RendererAPI::API::OpenGL    :return new OpenGLVertexArray();
        }
        MOL_CORE_ASSERT(false,"Unknown RendererAPI");
        return nullptr;
    }
}
