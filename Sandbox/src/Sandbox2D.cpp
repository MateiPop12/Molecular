#include "Sandbox2D.h"

#include "../../Molecular/vendor/imgui/imgui.h"

Sandbox2D::Sandbox2D()
    : Layer("Sandbox2D"), m_cameraController(1280.0f/720.0f, true)
{
}

void Sandbox2D::OnAttach()
{
    // Create water molecule (H2O)
    m_physicsWorld.AddObject(glm::vec2(1.01f, 1.68f), "O");  // Oxygen
    m_physicsWorld.AddObject(glm::vec2(0.43f, -0.14f), "H"); // Hydrogen
    m_physicsWorld.AddObject(glm::vec2(0.84f, 0.83f), "H");  // Hydrogen

    // Add covalent bonds
    m_physicsWorld.AddBond(0, 1, 0.96f, 0.5f); // O-H bond (Water molecule)
    m_physicsWorld.AddBond(0, 2, 0.96f, 0.5f); // O-H bond (Water molecule)
}

void Sandbox2D::OnDetach()
{
}

void Sandbox2D::OnUpdate(Molecular::Timestep ts)
{
    m_cameraController.OnUpdate(ts);

    float timeScale = 1.0f;
    float boxSize = 5.0f;
    m_physicsWorld.Update(ts.GetSeconds() * timeScale, boxSize);

    Molecular::RenderCommand::SetClearColor({ 0.15f, 0.15f, 0.15f, 1.0f });
    Molecular::RenderCommand::Clear();

    Molecular::Renderer2D::BeginScene(m_cameraController.GetCamera());

    float halfBox = boxSize / 2.0f;
    float borderThickness = 0.05f; // Thin white border

    // Top border
    Molecular::Renderer2D::DrawQuad(glm::vec2(0.0f, halfBox + 0.5), glm::vec2(boxSize + borderThickness + 1, borderThickness), {1.0f,1.0f,1.0f,1.0f});
    // Bottom border
    Molecular::Renderer2D::DrawQuad(glm::vec2(0.0f, -halfBox - + 0.5), glm::vec2(boxSize + borderThickness + 1, borderThickness), {1.0f,1.0f,1.0f,1.0f});
    // Left border
    Molecular::Renderer2D::DrawQuad(glm::vec2(-halfBox - 0.5, 0.0f), glm::vec2(borderThickness, boxSize + 1), {1.0f,1.0f,1.0f,1.0f});
    // Right border
    Molecular::Renderer2D::DrawQuad(glm::vec2(halfBox + 0.5 , 0.0f), glm::vec2(borderThickness, boxSize + 1), {1.0f,1.0f,1.0f,1.0f});

    for (const auto& obj : m_physicsWorld.GetObjects())
        Molecular::Renderer2D::DrawCircle(obj.GetPosition(), obj.GetRadius() * 0.2, obj.GetColor());

    for (const auto& bond : m_physicsWorld.GetBonds()) {
        const auto& objA = m_physicsWorld.GetObjects()[bond.atomA];
        const auto& objB = m_physicsWorld.GetObjects()[bond.atomB];

        Molecular::Renderer2D::DrawQuad(objA.GetPosition(), objB.GetPosition(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    }

    Molecular::Renderer2D::EndScene();

}

void Sandbox2D::OnImGuiRender()
{
    ImGui::Begin("Atom Properties");

    const auto& objects = m_physicsWorld.GetObjects();

    for (size_t i = 0; i < objects.size(); ++i)
    {
        const auto& obj = objects[i];

        ImGui::Separator();
        ImGui::Text("Atom %d", (int)i);
        ImGui::Text("Position: (%.2f, %.2f)", obj.GetPosition().x, obj.GetPosition().y);
        ImGui::Text("Mass: %.2f", obj.GetMass());
        ImGui::Text("Radius: %.2f", obj.GetRadius());
        ImGui::Text("Sigma: %.2f", obj.GetSigma());
        ImGui::Text("Epsilon: %.2f", obj.GetEpsilon());
    }
    ImGui::End();
}

void Sandbox2D::OnEvent(Molecular::Event& e)
{
    m_cameraController.OnEvent(e);
}
