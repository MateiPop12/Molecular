#include "Renderer3D.h"

#include <ext/matrix_transform.hpp>

#include "EditorCamera.h"
#include "RenderCommand.h"
#include "Shader.h"
#include "VertexArray.h"

namespace Molecular
{
    struct Renderer3DStorage
    {
        Ref<VertexArray> sphereVertexArray;
        Ref<Shader> sphereShader;

        Ref<VertexArray> boxVertexArray;
        Ref<Shader> boxShader;
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

        // ---------------- BOX SETUP ----------------
        renderer3DStorage->boxVertexArray = VertexArray::Create();

        // Unit box vertices (1x1x1 centered at origin)
        // Each vertex has position (x, y, z) and normal (nx, ny, nz)
        std::vector<float> boxVertices = {
            // Front face (z = 0.5)
            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  // Bottom-left
             0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  // Bottom-right
             0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  // Top-right
            -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  // Top-left

            // Back face (z = -0.5)
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  // Bottom-left
             0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  // Bottom-right
             0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  // Top-right
            -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  // Top-left

            // Left face (x = -0.5)
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  // Bottom-back
            -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  // Bottom-front
            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  // Top-front
            -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  // Top-back

            // Right face (x = 0.5)
             0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  // Bottom-back
             0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  // Bottom-front
             0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  // Top-front
             0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  // Top-back

            // Bottom face (y = -0.5)
            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  // Back-left
             0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  // Back-right
             0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  // Front-right
            -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  // Front-left

            // Top face (y = 0.5)
            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  // Back-left
             0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  // Back-right
             0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  // Front-right
            -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f   // Front-left
        };

        // Box indices (2 triangles per face, 6 faces)
        std::vector<uint32_t> boxIndices = {
            // Front face
            0, 1, 2,   2, 3, 0,
            // Back face
            4, 6, 5,   6, 4, 7,
            // Left face
            8, 9, 10,  10, 11, 8,
            // Right face
            12, 14, 13, 14, 12, 15,
            // Bottom face
            16, 17, 18, 18, 19, 16,
            // Top face
            20, 22, 21, 22, 20, 23
        };

        Ref<VertexBuffer> boxVB;
        boxVB.reset(VertexBuffer::Create(boxVertices.data(), boxVertices.size() * sizeof(float)));
        boxVB->SetLayout({
            { ShaderDataType::Float3, "a_Position" },
            { ShaderDataType::Float3, "a_Normal" }
        });

        renderer3DStorage->boxVertexArray->AddVertexBuffer(boxVB);

        Ref<IndexBuffer> boxIB;
        boxIB.reset(IndexBuffer::Create(boxIndices.data(), boxIndices.size()));
        renderer3DStorage->boxVertexArray->SetIndexBuffer(boxIB);

        // ---------------- SHADER SETUP ----------------
        renderer3DStorage->sphereShader = Shader::Create("D:/FACULTATE/Licenta/Molecular/Sandbox/assets/shaders/Sphere.glsl");
        renderer3DStorage->boxShader = Shader::Create("D:/FACULTATE/Licenta/Molecular/Sandbox/assets/shaders/Box.glsl");
    }

    void Renderer3D::Shutdown()
    {
        delete renderer3DStorage;
    }

    void Renderer3D::BeginScene(const EditorCamera& camera)
    {
        const glm::mat4 viewProjection = camera.GetViewProjection();

        renderer3DStorage->sphereShader->Bind();
        renderer3DStorage->sphereShader->SetMat4("u_ViewProjection", viewProjection);

        renderer3DStorage->boxShader->Bind();
        renderer3DStorage->boxShader->SetMat4("u_ViewProjection", viewProjection);
    }

    void Renderer3D::EndScene()
    {
    }

    void Renderer3D::DrawSphere(const glm::vec3& position, const float radius, const glm::vec4& color)
    {
        renderer3DStorage->sphereShader->Bind();
        renderer3DStorage->sphereShader->SetFloat4("u_Color", color);

        const glm::mat4 transform = translate(glm::mat4(1.0f), position)
            * scale(glm::mat4(1.0f), { radius * 2.0f, radius * 2.0f, radius * 2.0f });

        renderer3DStorage->sphereShader->SetMat4("u_Transform", transform);

        renderer3DStorage->sphereVertexArray->Bind();
        RenderCommand::DrawIndexed(renderer3DStorage->sphereVertexArray);
    }

    void Renderer3D::DrawCube(const glm::vec3& position, float size, const glm::vec4& color)
    {
        DrawBox(position, glm::vec3(size, size, size), color);
    }

    void Renderer3D::DrawBox(const glm::vec3& position, const glm::vec3& size, const glm::vec4& color)
    {
        renderer3DStorage->boxShader->Bind();
        renderer3DStorage->boxShader->SetFloat4("u_Color", color);

        const glm::mat4 transform = translate(glm::mat4(1.0f), position)
            * scale(glm::mat4(1.0f), size);

        renderer3DStorage->boxShader->SetMat4("u_Transform", transform);

        renderer3DStorage->boxVertexArray->Bind();
        RenderCommand::DrawIndexed(renderer3DStorage->boxVertexArray);
    }
}
