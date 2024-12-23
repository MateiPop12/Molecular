#pragma once

#include "Molecular/Renderer/VertexArray.h"

namespace Molecular
{
    class OpenGLVertexArray : public VertexArray
    {
    public:
        OpenGLVertexArray();
        virtual ~OpenGLVertexArray();

        void Bind() const override;
        void Unbind() const override;

        void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) override;
        void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) override;

        virtual const std::vector<std::shared_ptr<VertexBuffer>>& GetVertexBuffer() const {return m_vertexBuffers;}
        virtual const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const {return m_indexBuffer;}

    private:
        uint32_t m_RendererID;
        std::vector<std::shared_ptr<VertexBuffer>> m_vertexBuffers;
        std::shared_ptr<IndexBuffer> m_indexBuffer;
    };
}