#include "Platform/OpenGL/OpenGLVertexArray.h"

#include "glad/glad.h"

namespace Molecular
{
    static GLenum ShaderDataTypeToGL(ShaderDataType type)
    {
        switch (type)
        {
        case ShaderDataType::Float:		return GL_FLOAT;
        case ShaderDataType::Float2:	return GL_FLOAT;
        case ShaderDataType::Float3:	return GL_FLOAT;
        case ShaderDataType::Float4:	return GL_FLOAT;
        case ShaderDataType::Mat3:		return GL_FLOAT;
        case ShaderDataType::Mat4:		return GL_FLOAT;
        case ShaderDataType::Int:		return GL_INT;
        case ShaderDataType::Int2:		return GL_INT;
        case ShaderDataType::Int3:		return GL_INT;
        case ShaderDataType::Int4:		return GL_INT;
        case ShaderDataType::Bool:		return GL_BOOL;
        }
        MOL_CORE_ASSERT(false, "Unknown ShaderDataType!");
        return 0;
    }

    OpenGLVertexArray::OpenGLVertexArray()
    {
        glCreateVertexArrays(1, &m_RendererID);
    }

    OpenGLVertexArray::~OpenGLVertexArray()
    {
        glDeleteVertexArrays(1, &m_RendererID);
    }

    void OpenGLVertexArray::Bind() const
    {
        glBindVertexArray(m_RendererID);
    }

    void OpenGLVertexArray::Unbind() const
    {
        glBindVertexArray(0);
    }

    void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
    {
        MOL_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex Buffer Layout Error!");

        glBindVertexArray(m_RendererID);
        vertexBuffer->Bind();

        uint32_t index = 0;
        for (const auto& element : vertexBuffer->GetLayout())
        {
            glEnableVertexAttribArray(index);
            glVertexAttribPointer(index,
                element.GetComponentCount(),
                ShaderDataTypeToGL(element.type),
                element.normalized ? GL_TRUE : GL_FALSE,
                vertexBuffer->GetLayout().GetStride(),
                (const void*)element.offset);
            index ++;
        }
        m_vertexBuffers.push_back(vertexBuffer);
    }

    void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
    {
        glBindVertexArray(m_RendererID);
        indexBuffer->Bind();

        m_indexBuffer = indexBuffer;
    }
}
