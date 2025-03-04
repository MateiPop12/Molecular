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

    // Add physics objects
    m_physicsWorld.AddObject(glm::vec2(0.5f, 1.0f), 1.0f);  // Light object
    m_physicsWorld.AddObject(glm::vec2(-1.0f, 2.0f), 2.0f); // Heavier object
}

void Sandbox2D::OnDetach()
{
}

void Sandbox2D::OnUpdate(Molecular::Timestep ts)
{
    m_cameraController.OnUpdate(ts);

    m_physicsWorld.ApplyGlobalForce(glm::vec2(0.0f, -9.8f));

    m_physicsWorld.Update(ts.GetSeconds());

    Molecular::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
    Molecular::RenderCommand::Clear();

    Molecular::Renderer2D::BeginScene(m_cameraController.GetCamera());

    for (const auto& obj : m_physicsWorld.GetObjects())
        Molecular::Renderer2D::DrawCircle(obj.GetPosition(), 0.25f, { 0.0f, 0.0f, 1.0f, 1.0f });

    Molecular::Renderer2D::EndScene();

}

void Sandbox2D::OnImGuiRender()
{
    ImGui::Begin("Settings");
    ImGui::Text("Physics Debug");
    for (size_t i = 0; i < m_physicsWorld.GetObjects().size(); ++i)
        ImGui::Text("Object %d Position: (%.2f, %.2f)", (int)i, m_physicsWorld.GetObjects()[i].GetPosition().x, m_physicsWorld.GetObjects()[i].GetPosition().y);

    ImGui::End();
}

void Sandbox2D::OnEvent(Molecular::Event& e)
{
    m_cameraController.OnEvent(e);
}
