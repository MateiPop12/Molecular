#pragma once

#include <glm.hpp>

#include "VertexArray.h"

namespace Molecular
{
    class RendererAPI
    {
    public:
           enum class API
           {
               None=0,OpenGL,
           };
        virtual void Clear()=0;
        virtual void SetClearColor(const glm::vec4& color) = 0;
        virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) = 0;

        static API GetAPI() {return s_API;}
    private:
        static API s_API;
    };
}
