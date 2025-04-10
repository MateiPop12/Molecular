#include "Sandbox2D.h"

#include "../../Molecular/vendor/imgui/imgui.h"
#include <random>

Sandbox2D::Sandbox2D()
    : Layer("Sandbox2D"), m_cameraController(1920.0f/1080.0f, true)
{
}

void Sandbox2D::OnAttach()
{

    m_simulationSpace.AddObject(Molecular::Atom("H", glm::vec2(-0.12f, 0.0f)));
    m_simulationSpace.AddObject(Molecular::Atom("O", glm::vec2(0.12f, 0.0f)));
    //m_simulationSpace.AddObject(Molecular::Atom("N", glm::vec2(0.0f, 0.4f)));
    m_simulationSpace.AddObject(Molecular::Atom("C", glm::vec2(0.9f, -0.43f)));
    //m_simulationSpace.AddObject(Molecular::Atom("H", glm::vec2(-0.23f, 0.23f)));
    m_simulationSpace.AddObject(Molecular::Atom("H", glm::vec2(0.5f, 0.4f)));
    //m_simulationSpace.AddObject(Molecular::Atom("H", glm::vec2(-0.5f, 0.64f)));
    m_simulationSpace.AddObject(Molecular::Atom("H", glm::vec2(0.4f, -0.84f)));
    m_simulationSpace.AddObject(Molecular::Atom("H", glm::vec2(1.4f, -1.4f)));

}

void Sandbox2D::OnDetach()
{
}

void Sandbox2D::OnUpdate(Molecular::Timestep ts)
{
    m_cameraController.OnUpdate(ts);

    float boundingBoxSize = 1.5f;
    Molecular::BoundingBox box = {glm::vec2(-boundingBoxSize, -boundingBoxSize), glm::vec2(boundingBoxSize, boundingBoxSize)};
    glm::vec2 minPoint = box.GetMinPoint();
    glm::vec2 maxPoint = box.GetMaxPoint();
    glm::vec4 color = {1.0f,1.0f,1.0f,1.0f};
    float sum = 0.0f;
    float numberOfAtoms = 0.0f;
    float averageRadius = 0.12f;

    m_simulationSpace.Update(ts,box);

    Molecular::RenderCommand::SetClearColor({ 0.15f, 0.15f, 0.15f, 1.0f });
    Molecular::RenderCommand::Clear();
    Molecular::Renderer2D::BeginScene(m_cameraController.GetCamera());

    Molecular::Renderer2D::DrawQuad({abs(maxPoint.x) - abs(minPoint.x) ,maxPoint.y + averageRadius},{abs(maxPoint.x) + abs(minPoint.x) + (2 * averageRadius),0.05f},color);//top
    Molecular::Renderer2D::DrawQuad({abs(maxPoint.x) - abs(minPoint.x) ,minPoint.y - averageRadius},{abs(maxPoint.x) + abs(minPoint.x) + (2 * averageRadius),0.05f},color);//bottom
    Molecular::Renderer2D::DrawQuad({maxPoint.x + averageRadius,abs(maxPoint.y) - abs(minPoint.y)},{0.05f,abs(maxPoint.y) + abs(minPoint.y) + (2 * averageRadius)},color);//right
    Molecular::Renderer2D::DrawQuad({minPoint.x - averageRadius,abs(maxPoint.y) - abs(minPoint.y)},{0.05f,abs(maxPoint.y) + abs(minPoint.y) + (2 * averageRadius)},color);//left

    for (const Molecular::Atom& atom : m_simulationSpace.GetObjects())
    {
        Molecular::Renderer2D::DrawCircle(atom.GetPosition(),atom.GetVanDerWaalsRadius(),atom.GetColor());

        sum += atom.GetVanDerWaalsRadius();
        numberOfAtoms++;
    }

    averageRadius = sum/numberOfAtoms;

    Molecular::Renderer2D::EndScene();

}

void Sandbox2D::OnImGuiRender()
{
    ImGui::Begin("Atom Properties");

    ImGui::Text("Integration Method");
    if (ImGui::RadioButton("Euler", m_simulationSpace.GetIntegrationMethod() == Molecular::IntegrationMethod::Euler)) {
        m_simulationSpace.SetIntegrationMethod(Molecular::IntegrationMethod::Euler);
    }
    if (ImGui::RadioButton("Runge-Kutta 4", m_simulationSpace.GetIntegrationMethod() == Molecular::IntegrationMethod::RungeKutta4)) {
        m_simulationSpace.SetIntegrationMethod(Molecular::IntegrationMethod::RungeKutta4);
    }

    double energyLoss = m_simulationSpace.GetEnergyLossFactor();
    float energyLossF = static_cast<float>(energyLoss);  // ImGui only works with float

    if (ImGui::SliderFloat("Energy Loss Factor", &energyLossF, 0.0f, 0.99f, "%.2f")) {
        m_simulationSpace.SetEnergyLossFactor(static_cast<double>(energyLossF));
    }

    // Access the total energy via m_simulationSpace
    float totalEnergy = m_simulationSpace.CalculateTotalEnergy();
    ImGui::Text("Total Energy: %.4f eV", totalEnergy);

    // Access the energy history and plot it via m_simulationSpace
    const auto& energyHistory = m_simulationSpace.GetEnergyHistory();
    if (!energyHistory.empty()) {
        ImGui::PlotLines("Energy Over Time", energyHistory.data(),
                         energyHistory.size(), 0, nullptr, 0.0f,
                         *std::max_element(energyHistory.begin(), energyHistory.end()),
                         ImVec2(0, 100));
    }


    const auto& objects = m_simulationSpace.GetObjects();

    for (size_t i = 0; i < objects.size(); ++i)
    {
        const auto& obj = objects[i];

        ImGui::Separator();
        ImGui::Text("Position: (%.2f, %.2f)", obj.GetPosition().x, obj.GetPosition().y);
        ImGui::Text("Velocity: (%.2f, %.2f)", obj.GetVelocity().x, obj.GetVelocity().y);
    }
    ImGui::End();
}

void Sandbox2D::OnEvent(Molecular::Event& e)
{
    m_cameraController.OnEvent(e);
}
