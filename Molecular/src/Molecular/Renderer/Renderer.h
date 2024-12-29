#pragma once
#include "OrthographicCamera.h"
#include "RendererAPI.h"
#include "Shader.h"

namespace Molecular
{
    class Renderer
    {
    public:

        static void Init();

        static void BeginScene(OrthographicCamera& camera);
        static void EndScene();
        static void Submit(const Ref<VertexArray>& vertexArray, const Ref<Shader>& shader, const glm::mat4& transform = glm::mat4(1.0f));

        static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

    private:
        struct SceneData
        {
            glm::mat4 viewProjectionMatrix;
        };

        static SceneData* m_sceneData;
    };
}
