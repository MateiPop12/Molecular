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
        Ref<VertexArray> quadVertexArray;
        Ref<VertexArray> circleVertexArray;
        Ref<Shader> quadShader;
        Ref<Shader> circleShader;
        Ref<Texture2D> whiteTexture;
    };

    static Renderer2DStorage* rendererStorage;

    void Renderer2D::Init()
    {
        rendererStorage = new Renderer2DStorage;


        // ---------------- QUAD (Rectangle) SETUP ----------------
        rendererStorage->quadVertexArray = VertexArray::Create();
        float quadVertices[5 * 4] = {
            -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
             0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
             0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
            -0.5f,  0.5f, 0.0f, 0.0f, 1.0f
        };

        Ref<VertexBuffer> quadVB;
        quadVB.reset(VertexBuffer::Create(quadVertices, sizeof(quadVertices)));
        quadVB->SetLayout ({
            { ShaderDataType::Float3, "a_Position" },
            { ShaderDataType::Float2, "a_TexCoord" }
        });

        rendererStorage->quadVertexArray->AddVertexBuffer(quadVB);

        uint32_t indices[6] = { 0, 1, 2, 2, 3, 0 };
        Ref<IndexBuffer> quadIB;
        quadIB.reset(IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
        rendererStorage->quadVertexArray->SetIndexBuffer(quadIB);

        // ---------------- CIRCLE SETUP ----------------

        rendererStorage->circleVertexArray = VertexArray::Create();

        std::vector<float> circleVertices;
        const int segments = 36; // More segments = smoother circle
        circleVertices.push_back(0.0f); // Center of the circle
        circleVertices.push_back(0.0f);
        circleVertices.push_back(0.0f);

        for (int i = 0; i <= segments; i++)
        {
            float angle = (float)i / (float)segments * 2.0f * glm::pi<float>();
            circleVertices.push_back(cos(angle) * 0.5f);
            circleVertices.push_back(sin(angle) * 0.5f);
            circleVertices.push_back(0.0f);
        }

        Ref<VertexBuffer> circleVB;
        circleVB.reset(VertexBuffer::Create(circleVertices.data(), circleVertices.size() * sizeof(float)));
        circleVB->SetLayout({
            { ShaderDataType::Float3, "a_Position" }
        });


        rendererStorage->circleVertexArray->AddVertexBuffer(circleVB);

        std::vector<uint32_t> circleIndices;
        for (int i = 1; i <= segments; i++)
        {
            circleIndices.push_back(0);
            circleIndices.push_back(i);
            circleIndices.push_back(i + 1);
        }
        circleIndices.push_back(0);
        circleIndices.push_back(segments);
        circleIndices.push_back(1);

        Ref<IndexBuffer> circleIB;
        circleIB.reset(IndexBuffer::Create(circleIndices.data(), circleIndices.size()));
        rendererStorage->circleVertexArray->SetIndexBuffer(circleIB);

        // ---------------- TEXTURE & SHADER SETUP ----------------
        rendererStorage->whiteTexture = Texture2D::Create(1, 1);
        uint32_t whiteTextureData = 0xffffffff;
        rendererStorage->whiteTexture->SetData(&whiteTextureData, sizeof(whiteTextureData));

        rendererStorage->quadShader = Shader::Create("D:/FACULTATE/Licenta/Molecular/Sandbox/assets/shaders/Texture.glsl");
        rendererStorage->quadShader->Bind();
        rendererStorage->quadShader->SetInt("u_Texture", 0);
    }

    void Renderer2D::Shutdown()
    {
    }

    void Renderer2D::BeginScene(const OrthographicCamera& camera)
    {
        rendererStorage->quadShader->Bind();
        rendererStorage->quadShader->SetMat4("u_ViewProjection", camera.getViewProjectionMatrix());
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
        rendererStorage->quadShader->SetFloat4("u_Color", color);
        rendererStorage->whiteTexture->Bind();

        glm::mat4 transform = translate(glm::mat4(1.0f), position)
        * scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

        rendererStorage->quadShader->SetMat4("u_Transform", transform);
        rendererStorage->quadShader->SetFloat("u_TilingMultiplier", 1.0f);

        rendererStorage->quadVertexArray->Bind();
        RenderCommand::DrawIndexed(rendererStorage->quadVertexArray);
    }

    void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tiling)
    {
        DrawQuad({position.x, position.y, 0.0f}, size, texture, tiling);
    }

    void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tiling)
    {
        rendererStorage->quadShader->SetFloat4("u_Color", glm::vec4(1.0f));
        texture->Bind();

        glm::mat4 transform = translate(glm::mat4(1.0f), position)
        * scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

        rendererStorage->quadShader->SetMat4("u_Transform", transform);
        rendererStorage->quadShader->SetFloat("u_TilingMultiplier", tiling);

        rendererStorage->quadVertexArray->Bind();
        RenderCommand::DrawIndexed(rendererStorage->quadVertexArray);
    }

    void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
    {
        DrawRotatedQuad({position.x, position.y, 0.0f}, size, rotation, color);
    }

    void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
    {
        rendererStorage->quadShader->SetFloat4("u_Color", color);
        rendererStorage->quadShader->SetFloat("u_TilingMultiplier", 1.0f);
        rendererStorage->whiteTexture->Bind();

        glm::mat4 transform = translate(glm::mat4(1.0f), position)
        * rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f })
        * scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

        rendererStorage->quadShader->SetMat4("u_Transform", transform);

        rendererStorage->quadVertexArray->Bind();
        RenderCommand::DrawIndexed(rendererStorage->quadVertexArray);
    }

    void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tiling)
    {
        DrawRotatedQuad({position.x, position.y, 0.0f}, size, rotation, texture, tiling);
    }

    void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tiling)
    {
        rendererStorage->quadShader->SetFloat4("u_Color", glm::vec4(1.0f));
        texture->Bind();

        glm::mat4 transform = translate(glm::mat4(1.0f), position)
        * rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f })
        * scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

        rendererStorage->quadShader->SetMat4("u_Transform", transform);
        rendererStorage->quadShader->SetFloat("u_TilingMultiplier", tiling);

        rendererStorage->quadVertexArray->Bind();
        RenderCommand::DrawIndexed(rendererStorage->quadVertexArray);
    }

    void Renderer2D::DrawCircle(const glm::vec2 &position, float radius, const glm::vec4 &color) {
        DrawCircle({position.x, position.y, 0.0f}, radius, color);
    }

    void Renderer2D::DrawCircle(const glm::vec3 &position, float radius, const glm::vec4 &color) {

        rendererStorage->quadShader->SetFloat4("u_Color", color);
        rendererStorage->whiteTexture->Bind();

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
                              * glm::scale(glm::mat4(1.0f), {radius * 2.0f, radius * 2.0f, 1.0f});

        rendererStorage->quadShader->SetMat4("u_Transform", transform);

        rendererStorage->circleVertexArray->Bind();
        RenderCommand::DrawIndexed(rendererStorage->circleVertexArray);
    }
}
