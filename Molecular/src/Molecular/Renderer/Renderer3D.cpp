#include "Renderer3D.h"

#include <ext/matrix_transform.hpp>

#include "RenderCommand.h"
#include "Shader.h"
#include "VertexArray.h"

namespace Molecular
{
    struct Renderer3DStorage
    {
        Ref<VertexArray> sphereVertexArray;
        Ref<Shader> sphereShader;
    };

    static Renderer3DStorage* renderer3DStorage;

    void Renderer3D::Init()
    {
        renderer3DStorage = new Renderer3DStorage();

        // ---------------- SPHERE SETUP ----------------
        renderer3DStorage->sphereVertexArray = VertexArray::Create();

        std::vector<float> sphereVertices;
        std::vector<uint32_t> sphereIndices;

        constexpr int rings = 16;
        constexpr int sectors = 32;

        constexpr float R = 1.0f / static_cast<float>(rings - 1);
        constexpr float S = 1.0f / static_cast<float>(sectors - 1);

        // Generate vertices
        for (int r = 0; r < rings; ++r) {
            for (int s = 0; s < sectors; ++s) {
                constexpr float radius = 1.0f;
                float const y = sin(-glm::pi<float>() / 2.0f + glm::pi<float>() * r * R);
                float const x = cos(2 * glm::pi<float>() * s * S) * sin(glm::pi<float>() * r * R);
                float const z = sin(2 * glm::pi<float>() * s * S) * sin(glm::pi<float>() * r * R);

                // Position
                sphereVertices.push_back(x * radius);
                sphereVertices.push_back(y * radius);
                sphereVertices.push_back(z * radius);

                // Normal (same as position for a unit sphere)
                sphereVertices.push_back(x);
                sphereVertices.push_back(y);
                sphereVertices.push_back(z);
            }
        }

        // Generate indices
        for (int r = 0; r < rings - 1; ++r) {
            for (int s = 0; s < sectors - 1; ++s) {
                int const curRow = r * sectors;
                int const nextRow = (r + 1) * sectors;

                // First triangle
                sphereIndices.push_back(curRow + s);
                sphereIndices.push_back(nextRow + s);
                sphereIndices.push_back(nextRow + (s + 1));

                // Second triangle
                sphereIndices.push_back(curRow + s);
                sphereIndices.push_back(nextRow + (s + 1));
                sphereIndices.push_back(curRow + (s + 1));
            }
        }

        Ref<VertexBuffer> sphereVB;
        sphereVB.reset(VertexBuffer::Create(sphereVertices.data(), sphereVertices.size() * sizeof(float)));
        sphereVB->SetLayout({
            { ShaderDataType::Float3, "a_Position" },
            { ShaderDataType::Float3, "a_Normal" }
        });

        renderer3DStorage->sphereVertexArray->AddVertexBuffer(sphereVB);

        Ref<IndexBuffer> sphereIB;
        sphereIB.reset(IndexBuffer::Create(sphereIndices.data(), sphereIndices.size()));
        renderer3DStorage->sphereVertexArray->SetIndexBuffer(sphereIB);

        // ---------------- SHADER SETUP ----------------
        renderer3DStorage->sphereShader = Shader::Create("D:/FACULTATE/Licenta/Molecular/Sandbox/assets/shaders/Sphere.glsl");
    }

    void Renderer3D::Shutdown()
    {
        delete renderer3DStorage;
    }

    void Renderer3D::BeginScene(const OrthographicCamera& camera)
    {
        renderer3DStorage->sphereShader->Bind();
        renderer3DStorage->sphereShader->SetMat4("u_ViewProjection", camera.getViewProjectionMatrix());
    }

    void Renderer3D::EndScene()
    {
    }

    void Renderer3D::DrawSphere(const glm::vec3& position, const float radius, const glm::vec4& color)
    {
        renderer3DStorage->sphereShader->SetFloat4("u_Color", color);

        const glm::mat4 transform = translate(glm::mat4(1.0f), position)
            * scale(glm::mat4(1.0f), { radius * 2.0f, radius * 2.0f, radius * 2.0f });

        renderer3DStorage->sphereShader->SetMat4("u_Transform", transform);

        renderer3DStorage->sphereVertexArray->Bind();
        RenderCommand::DrawIndexed(renderer3DStorage->sphereVertexArray);
    }


}