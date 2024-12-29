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
        void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) override;
    };
}