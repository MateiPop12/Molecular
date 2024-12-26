#pragma once

#include "Molecular/Renderer/RendererAPI.h"

namespace Molecular
{
    class OpenGLRenderer : public RendererAPI
    {
        public:
        void Clear() override;
        void SetClearColor(const glm::vec4& color) override;
        void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) override;
    };
}