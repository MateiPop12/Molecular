#include  "molpch.h"
#include "Molecular/Renderer/Buffer.h"

#include "Core.h"
#include "Molecular/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Molecular
{

    VertexBuffer* VertexBuffer::Create(float* vertices, uint32_t size)
    {
        switch (Renderer::GetAPI())
        {
            case RendererAPI::None      :MOL_CORE_ASSERT(false,"RendererAPI::None not supported"); return nullptr;
            case RendererAPI::OpenGL    :return new OpenGLVertexBuffer(vertices, size);
        }
        MOL_CORE_ASSERT(false,"Unknown RendererAPI");
        return nullptr;
    }

    IndexBuffer* IndexBuffer::Create(uint32_t* indices, uint32_t size)
    {
        switch (Renderer::GetAPI())
        {
        case RendererAPI::None      :MOL_CORE_ASSERT(false,"RendererAPI::None not supported"); return nullptr;
        case RendererAPI::OpenGL    :return new OpenGLIndexBuffer(indices, size);
        }
        MOL_CORE_ASSERT(false,"Unknown RendererAPI");
        return nullptr;
    }
}
