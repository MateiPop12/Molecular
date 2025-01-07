#include "Renderer2D.h"

#include <ext/matrix_transform.hpp>

#include "RenderCommand.h"
#include "Shader.h"
#include "VertexArray.h"
#include "Core/Core.h"

namespace Molecular
{
    struct Renderer2DStorage
    {
        Ref<VertexArray> vertexArray;
        Ref<Shader> textureShader;
        Ref<Texture2D> whiteTexture;
    };

    static Renderer2DStorage* rendererStorage;

    void Renderer2D::Init()
    {
        rendererStorage = new Renderer2DStorage;

        rendererStorage->vertexArray = VertexArray::Create();
        float vertices[5*4] = {
            -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
             0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
             0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
            -0.5f,  0.5f, 0.0f, 0.0f, 1.0f
        };

        Ref<VertexBuffer> VB;
        VB.reset(VertexBuffer::Create(vertices, sizeof(vertices)));
        BufferLayout layout = {
            { ShaderDataType::Float3, "a_Position" },
            { ShaderDataType::Float2, "a_TexCoord" }
        };

        VB->SetLayout(layout);
        rendererStorage->vertexArray->AddVertexBuffer(VB);

        uint32_t indices[6] = { 0, 1, 2, 2, 3, 0 };
        Ref<IndexBuffer> IB;
        IB.reset(IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
        rendererStorage->vertexArray->SetIndexBuffer(IB);

        rendererStorage->whiteTexture = Texture2D::Create(1, 1);
        uint32_t whiteTextureData = 0xffffffff;
        rendererStorage->whiteTexture->SetData(&whiteTextureData, sizeof(whiteTextureData));

        rendererStorage->textureShader = Shader::Create("D:/FACULTATE/Licenta/Molecular/Sandbox/assets/shaders/Texture.glsl");
        rendererStorage->textureShader->Bind();
        rendererStorage->textureShader->SetInt("u_Texture",0);

    }

    void Renderer2D::Shutdown()
    {
    }

    void Renderer2D::BeginScene(const OrthographicCamera& camera)
    {
        rendererStorage->textureShader->Bind();
        rendererStorage->textureShader->SetMat4("u_ViewProjection", camera.getViewProjectionMatrix());
    }

    void Renderer2D::EndScene()
    {
    }

    void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
    {
       DrawQuad({position.x, position.y, 0.0f}, size, color);
    }

    void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
    {
        rendererStorage->textureShader->SetFloat4("u_Color", color);
        rendererStorage->whiteTexture->Bind();

        glm::mat4 transform = translate(glm::mat4(1.0f), position)
        * scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

        rendererStorage->textureShader->SetMat4("u_Transform", transform);
        rendererStorage->textureShader->SetFloat("u_TilingMultiplier", 1.0f);

        rendererStorage->vertexArray->Bind();
        RenderCommand::DrawIndexed(rendererStorage->vertexArray);
    }

    void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tiling)
    {
        DrawQuad({position.x, position.y, 0.0f}, size, texture, tiling);
    }

    void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tiling)
    {
        rendererStorage->textureShader->SetFloat4("u_Color", glm::vec4(1.0f));
        texture->Bind();

        glm::mat4 transform = translate(glm::mat4(1.0f), position)
        * scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

        rendererStorage->textureShader->SetMat4("u_Transform", transform);
        rendererStorage->textureShader->SetFloat("u_TilingMultiplier", tiling);

        rendererStorage->vertexArray->Bind();
        RenderCommand::DrawIndexed(rendererStorage->vertexArray);
    }

    void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
    {
        DrawRotatedQuad({position.x, position.y, 0.0f}, size, rotation, color);
    }

    void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
    {
        rendererStorage->textureShader->SetFloat4("u_Color", color);
        rendererStorage->textureShader->SetFloat("u_TilingMultiplier", 1.0f);
        rendererStorage->whiteTexture->Bind();

        glm::mat4 transform = translate(glm::mat4(1.0f), position)
        * rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f })
        * scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

        rendererStorage->textureShader->SetMat4("u_Transform", transform);

        rendererStorage->vertexArray->Bind();
        RenderCommand::DrawIndexed(rendererStorage->vertexArray);
    }

    void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tiling)
    {
        DrawRotatedQuad({position.x, position.y, 0.0f}, size, rotation, texture, tiling);
    }

    void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tiling)
    {
        rendererStorage->textureShader->SetFloat4("u_Color", glm::vec4(1.0f));
        texture->Bind();

        glm::mat4 transform = translate(glm::mat4(1.0f), position)
        * rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f })
        * scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

        rendererStorage->textureShader->SetMat4("u_Transform", transform);
        rendererStorage->textureShader->SetFloat("u_TilingMultiplier", tiling);

        rendererStorage->vertexArray->Bind();
        RenderCommand::DrawIndexed(rendererStorage->vertexArray);
    }
}
