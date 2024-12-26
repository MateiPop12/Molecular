#pragma once
#include "OrthographicCamera.h"
#include "RendererAPI.h"
#include "Shader.h"

namespace Molecular
{
    class Renderer
    {
    public:
        static void BeginScene(OrthographicCamera& camera);
        static void EndScene();
        static void Submit(const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Shader>& shader);

        static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

    private:
        struct SceneData
        {
            glm::mat4 viewProjectionMatrix;
        };

        static SceneData* m_sceneData;
    };
}
