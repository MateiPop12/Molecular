#pragma once

#include "Molecular/Renderer/RendererAPI.h"

namespace Molecular
{
    class OpenGLRendererAPI : public RendererAPI
    {
        public:

        void Init() override;

        void Clear() override;
        void SetClearColor(const glm::vec4& color) override;
        void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
        void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) override;
    };
}