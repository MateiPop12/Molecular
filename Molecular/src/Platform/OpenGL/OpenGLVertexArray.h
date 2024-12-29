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

        void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) override;
        void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) override;

        virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffer() const {return m_vertexBuffers;}
        virtual const Ref<IndexBuffer>& GetIndexBuffer() const {return m_indexBuffer;}

    private:
        uint32_t m_RendererID;
        std::vector<Ref<VertexBuffer>> m_vertexBuffers;
        Ref<IndexBuffer> m_indexBuffer;
    };
}