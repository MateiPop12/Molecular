#include "Sandbox2D.h"

#include <gtc/type_ptr.hpp>
#include "../../Molecular/vendor/imgui/imgui.h"

Sandbox2D::Sandbox2D()
    : Layer("Sandbox2D"), m_cameraController(1280.0f/720.0f, true)
{
}

void Sandbox2D::OnAttach()
{
    m_texture = Molecular::Texture2D::Create("D:/FACULTATE/Licenta/Molecular/Sandbox/assets/textures/Checkerboard.png");
}

void Sandbox2D::OnDetach()
{
}

void Sandbox2D::OnUpdate(Molecular::Timestep ts)
{
    m_cameraController.OnUpdate(ts);

    Molecular::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
    Molecular::RenderCommand::Clear();

    Molecular::Renderer2D::BeginScene(m_cameraController.GetCamera());

    Molecular::Renderer2D::DrawQuad({ 0.5f, 0.5f }, { 0.3f, 1.5f }, { 1.0f, 0.0f, 0.0f , 1.0f});
    Molecular::Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 0.5f, 0.5f }, { 0.0f, 1.0f, 0.0f , 1.0f});
    Molecular::Renderer2D::DrawRotatedQuad({ 1.0f, -0.5f, -0.1f }, { 10.0f, 10.0f }, glm::radians(45.0f), m_texture, 20.0f);

    Molecular::Renderer2D::EndScene();

}

void Sandbox2D::OnImGuiRender()
{
    ImGui::Begin("Settings");
    ImGui::ColorEdit3("Square Color", value_ptr(m_objectColor));
    ImGui::End();
}

void Sandbox2D::OnEvent(Molecular::Event& e)
{
    m_cameraController.OnEvent(e);
}
