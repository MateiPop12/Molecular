#pragma once

#include "RendererAPI.h"

namespace Molecular
{
    class RenderCommand
    {
    public:

        static void Init()
        {
            s_RendererAPI->Init();
        }
        static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
        {
            s_RendererAPI->SetViewport(x,y,width,height);
        }
        static void Clear()
        {
            s_RendererAPI->Clear();
        }
        static void SetClearColor(const glm::vec4& color)
        {
            s_RendererAPI->SetClearColor(color);
        }

        static void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray)
        {
            s_RendererAPI->DrawIndexed(vertexArray);
        }
    private:
        static RendererAPI* s_RendererAPI;
    };

}