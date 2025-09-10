#include "Sandbox3D.h"
#include "../../Molecular/vendor/imgui/imgui.h"

Sandbox3D::Sandbox3D()
: Layer("SandBox3D"), m_CameraController(45.0f, 1280.0f / 720.0f, 0.1f, 1000.0f),
  m_ViewportBounds{}
{
}

Sandbox3D::~Sandbox3D()
= default;

void Sandbox3D::OnAttach()
{
}

void Sandbox3D::OnDetach(){}

void Sandbox3D::OnUpdate(const Molecular::Timestep ts)
{
    m_CameraController.OnUpdate(ts);

    Molecular::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
    Molecular::RenderCommand::Clear();


    Molecular::Renderer3D::BeginScene(m_CameraController);

    DrawSpace(0,10.0f);

    Molecular::Renderer3D::DrawSphere({0.0f, 0.0f, 0.0f},  0.1f, {0.8f, 0.2f, 0.2f, 1.0f});
    Molecular::Renderer3D::DrawSphere({1.0f, 1.0f, 1.0f}, 0.5f, {0.2f, 0.2f, 0.8f, 1.0f});

    Molecular::Renderer::EndScene();

}

void Sandbox3D::OnImGuiRender()
{
}

void Sandbox3D::OnEvent(Molecular::Event& event)
{
    m_CameraController.OnEvent(event);
}

void Sandbox3D::DrawSpace(const short type, float size)
{
    const float half = size / 2.0f;
    constexpr float thickness = 0.1f;
    constexpr glm::vec4 color= {0.9f, 0.9f, 0.9f, 1.0f};

    if (type == 0)
    {
        // Bottom edges
        Molecular::Renderer3D::DrawBox({0, -half, -half}, {size, thickness, thickness}, color);
        Molecular::Renderer3D::DrawBox({0, -half,  half}, {size, thickness, thickness}, color);
        Molecular::Renderer3D::DrawBox({ half, -half, 0}, {thickness, thickness, size}, color);
        Molecular::Renderer3D::DrawBox({-half, -half, 0}, {thickness, thickness, size}, color);

        // Vertical edges
        Molecular::Renderer3D::DrawBox({-half, 0,  half}, {thickness, size, thickness}, color);
        Molecular::Renderer3D::DrawBox({-half, 0, -half}, {thickness, size, thickness}, color);
        Molecular::Renderer3D::DrawBox({ half, 0, -half}, {thickness, size, thickness}, color);

        // Top edges
        Molecular::Renderer3D::DrawBox({0,  half, -half}, {size, thickness, thickness}, color);
        Molecular::Renderer3D::DrawBox({-half,  half, 0}, {thickness, thickness, size}, color);
    }
    else if (type == 1)
    {
        Molecular::Renderer3D::DrawBox({0,    0, -half}, {size, size, thickness}, color);
        Molecular::Renderer3D::DrawBox({0, -half,    0}, {size, thickness, size}, color);
        Molecular::Renderer3D::DrawBox({-half, 0,    0}, {thickness, size, size}, color);
    }
}
