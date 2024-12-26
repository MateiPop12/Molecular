#include "Molecular/Renderer/Renderer.h"

#include "RenderCommand.h"

namespace Molecular
{
    Renderer::SceneData* Renderer::m_sceneData = new SceneData;

    void Renderer::BeginScene(OrthographicCamera& camera)
    {
        m_sceneData->viewProjectionMatrix = camera.getViewProjectionMatrix();
    }

    void Renderer::EndScene()
    {
    }

    void Renderer::Submit(const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Shader>& shader)
    {
        shader->Bind();
        shader->UploadUniformMat4("u_ViewProjection", m_sceneData->viewProjectionMatrix);

        vertexArray->Bind();
        RenderCommand::DrawIndexed(vertexArray);
    }
}
