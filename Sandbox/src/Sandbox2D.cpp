#include "Sandbox2D.h"

#include "Molecular.h"
#include "../../Molecular/vendor/imgui/imgui.h"
#include <random>

Sandbox2D::Sandbox2D()
    : Layer("Sandbox2D"), m_cameraController(1920.0f/1080.0f, true),
      m_rng(std::random_device{}()),
      m_positionDistribution(-m_boundingBoxSize + 0.2f, m_boundingBoxSize - 0.2f)
{
    for (const auto& element : m_availableElements) {
        m_atomCounts[element] = 0;
    }
}

void Sandbox2D::OnAttach()
{
    UpdateAtomCounts();
}

void Sandbox2D::OnDetach()
{
}

void Sandbox2D::OnUpdate(Molecular::Timestep ts)
{
    m_cameraController.OnUpdate(ts);

    Molecular::BoundingBox box = {glm::vec2(-m_boundingBoxSize, -m_boundingBoxSize), glm::vec2(m_boundingBoxSize, m_boundingBoxSize)};
    glm::vec2 minPoint = box.GetMinPoint();
    glm::vec2 maxPoint = box.GetMaxPoint();
    glm::vec4 color = {1.0f,1.0f,1.0f,1.0f};
    float sum = 0.0f;
    float numberOfAtoms = 0.0f;
    float averageRadius = 0.12f;

    m_simulationSpace.Update(ts,box);
    m_simulationSpace.UpdateBonds();

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
    ImGui::Begin("Simulation Controls");

    // === SIMULATION CONTROL SECTION ===
    ImGui::SeparatorText("Simulation Control");

    bool isRunning = m_simulationSpace.IsRunning();

    // Start/Stop buttons
    if (isRunning) {
        if (ImGui::Button("Stop Simulation", ImVec2(120, 30))) {
            m_simulationSpace.StopSimulation();
        }
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Running");
    } else {
        if (ImGui::Button("Start Simulation", ImVec2(120, 30))) {
            m_simulationSpace.StartSimulation();
        }
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Stopped");
    }

    ImGui::Spacing();

    // Reset buttons
    if (ImGui::Button("Reset Simulation", ImVec2(120, 30))) {
        ResetSimulation();
    }
    ImGui::SameLine();
    if (ImGui::Button("Clear All", ImVec2(120, 30))) {
        m_simulationSpace.ClearAllAtoms();
        UpdateAtomCounts();
    }
    ImGui::SameLine();
    if (ImGui::Button("Default Setup", ImVec2(120, 30))) {
        SetupDefaultSimulation();
    }

    ImGui::Spacing();

    // === ATOM MANAGEMENT SECTION ===
    ImGui::SeparatorText("Atom Management");

    if (isRunning) {
        ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.0f, 1.0f), "Stop simulation to add/remove atoms");
    } else {
        ImGui::Text("Add/Remove Atoms:");

        for (const auto& element : m_availableElements) {
            ImGui::PushID(element.c_str());

            // Display element name and count
            ImGui::Text("%s (%d):", element.c_str(), GetAtomCount(element));
            ImGui::SameLine();

            // Add button
            if (ImGui::Button("+")) {
                AddRandomAtom(element);
            }
            ImGui::SameLine();

            // Remove button (only enabled if there are atoms of this type)
            if (GetAtomCount(element) > 0) {
                if (ImGui::Button("-")) {
                    RemoveLastAtom(element);
                }
            } else {
                ImGui::BeginDisabled();
                ImGui::Button("-");
                ImGui::EndDisabled();
            }

            ImGui::PopID();
        }
    }

    ImGui::Spacing();

    // === SIMULATION PARAMETERS SECTION ===
    ImGui::SeparatorText("Simulation Parameters");

    ImGui::Text("Integration Method");
    if (ImGui::RadioButton("Euler", m_simulationSpace.GetIntegrationMethod() == Molecular::IntegrationMethod::Euler)) {
        m_simulationSpace.SetIntegrationMethod(Molecular::IntegrationMethod::Euler);
    }
    if (ImGui::RadioButton("Runge-Kutta 4", m_simulationSpace.GetIntegrationMethod() == Molecular::IntegrationMethod::RungeKutta4)) {
        m_simulationSpace.SetIntegrationMethod(Molecular::IntegrationMethod::RungeKutta4);
    }

    double energyLoss = m_simulationSpace.GetEnergyLossFactor();
    float energyLossF = static_cast<float>(energyLoss);

    if (ImGui::SliderFloat("Energy Loss Factor", &energyLossF, 0.0f, 0.99f, "%.2f")) {
        m_simulationSpace.SetEnergyLossFactor(static_cast<double>(energyLossF));
    }

    ImGui::SeparatorText("Simulation Boundaries");

    if (isRunning) {
        // Show current value but disable modification when simulation is running
        ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.0f, 1.0f), "Stop simulation to modify boundaries");
        ImGui::BeginDisabled();
    }

    float boundingBoxSize = m_boundingBoxSize;
    // Custom slider with 0.2 increments between 0.4 and 10.0
    if (ImGui::SliderFloat("Bounding Box Size", &boundingBoxSize, 0.4f, 10.0f, "%.1f")) {
        // Round to nearest 0.2 increment
        boundingBoxSize = std::round(boundingBoxSize / 0.2f) * 0.2f;
        // Clamp to ensure we stay within bounds after rounding
        boundingBoxSize = std::clamp(boundingBoxSize, 0.4f, 5.0f);

        m_boundingBoxSize = boundingBoxSize;

        // Update the position distribution range when bounding box changes
        m_positionDistribution = std::uniform_real_distribution<float>(
            -m_boundingBoxSize + 0.2f,
            m_boundingBoxSize - 0.2f
        );
    }

    if (isRunning) {
        ImGui::EndDisabled();
    }

    ImGui::Text("Current bounds: [%.1f, %.1f] x [%.1f, %.1f]",
        -m_boundingBoxSize, m_boundingBoxSize,
        -m_boundingBoxSize, m_boundingBoxSize);

    ImGui::Spacing();

    // === ENERGY MONITORING SECTION ===
    ImGui::SeparatorText("Energy Monitoring");

    float totalEnergy = m_simulationSpace.CalculateTotalEnergy();
    ImGui::Text("Total Energy: %.4f eV", totalEnergy);

    const auto& energyHistory = m_simulationSpace.GetEnergyHistory();
    ImGui::Text("Data Points: %zu", energyHistory.size());

    if (!energyHistory.empty()) {
        ImGui::PlotLines("Energy Over Time", energyHistory.data(),
                         energyHistory.size(), 0, nullptr, 0.0f,
                         *std::max_element(energyHistory.begin(), energyHistory.end()),
                         ImVec2(0, 100));
    }

    ImGui::Spacing();
    if (ImGui::Button("Export Energy Data", ImVec2(150, 30))) {
        m_simulationSpace.ExportEnergyDataToCSV();
    }
    ImGui::SameLine();
    if (ImGui::Button("Clear History", ImVec2(120, 30))) {
        m_simulationSpace.ClearEnergyHistory();
    }

    ImGui::Spacing();

    // === BOND INFORMATION SECTION ===
    ImGui::SeparatorText("Bond Information");

    int totalBonds = m_simulationSpace.GetTotalBondCount();
    ImGui::Text("Total Bonds: %d", totalBonds);

    // Show individual atom bond counts
    const auto& atoms = m_simulationSpace.GetObjects();
    for (size_t i = 0; i < atoms.size(); ++i) {
        const auto& atom = atoms[i];
        int bondCount = atom.GetBonds().size();
        int maxBonds = atom.GetValence();

        ImGui::Text("Atom %zu (%s): %d/%d bonds", i, atom.GetElement().c_str(), bondCount, maxBonds);

        // Show what it's bonded to
        if (bondCount > 0) {
            ImGui::SameLine();
            ImGui::Text("-> ");
            for (const auto* bondedAtom : atom.GetBonds()) {
                // Find the index of the bonded atom
                for (size_t j = 0; j < atoms.size(); ++j) {
                    if (&atoms[j] == bondedAtom) {
                        ImGui::SameLine();
                        ImGui::Text("%s%zu", j > 0 ? ", " : "", j);
                        break;
                    }
                }
            }
        }
    }

    ImGui::Spacing();

    // === ATOM PROPERTIES SECTION ===
    ImGui::SeparatorText("Atom Properties");

    const auto& objects = m_simulationSpace.GetObjects();

    for (size_t i = 0; i < objects.size(); ++i)
    {
        const auto& obj = objects[i];

        ImGui::PushID(static_cast<int>(i));

        if (ImGui::CollapsingHeader(("Atom " + std::to_string(i) + " (" + obj.GetElement() + ")").c_str())) {
            ImGui::Text("Position: (%.2f, %.2f)", obj.GetPosition().x, obj.GetPosition().y);
            ImGui::Text("Velocity: (%.2f, %.2f)", obj.GetVelocity().x, obj.GetVelocity().y);
            ImGui::Text("Mass: %.3f amu", obj.GetMassD());
            ImGui::Text("Charge: %.2f e", obj.GetCharge());
        }

        ImGui::PopID();
    }

    ImGui::End();
}

void Sandbox2D::OnEvent(Molecular::Event& e)
{
    m_cameraController.OnEvent(e);
}

void Sandbox2D::AddRandomAtom(const std::string& elementType)
{
    glm::vec2 position = GenerateRandomPosition();

    // Try to find a valid position (max 100 attempts to avoid infinite loop)
    int attempts = 0;
    while (!IsPositionValid(position, 0.12f) && attempts < 100) {
        position = GenerateRandomPosition();
        attempts++;
    }

    if (attempts < 100) {
        m_simulationSpace.AddObject(Molecular::Atom(elementType, position));
        m_atomCounts[elementType]++;
    }
    // If we couldn't find a valid position after 100 attempts, we don't add the atom
}

void Sandbox2D::RemoveLastAtom(const std::string& elementType)
{
    auto& atoms = m_simulationSpace.GetObjectsMutable();

    // Find the last atom of the specified type and remove it
    for (auto it = atoms.rbegin(); it != atoms.rend(); ++it) {
        if (it->GetElement() == elementType) {
            atoms.erase(std::next(it).base());
            m_atomCounts[elementType]--;
            break;
        }
    }
}

glm::vec2 Sandbox2D::GenerateRandomPosition()
{
    float x = m_positionDistribution(m_rng);
    float y = m_positionDistribution(m_rng);
    return glm::vec2(x, y);
}

bool Sandbox2D::IsPositionValid(const glm::vec2& position, float radius)
{
    const auto& atoms = m_simulationSpace.GetObjects();

    for (const auto& atom : atoms) {
        float distance = glm::length(position - atom.GetPosition());
        float minDistance = radius + atom.GetVanDerWaalsRadius();

        if (distance < minDistance) {
            return false;
        }
    }

    return true;
}

void Sandbox2D::UpdateAtomCounts()
{
    for (auto& pair : m_atomCounts) {
        pair.second = 0;
    }

    const auto& atoms = m_simulationSpace.GetObjects();
    for (const auto& atom : atoms) {
        m_atomCounts[atom.GetElement()]++;
    }
}

int Sandbox2D::GetAtomCount(const std::string& elementType) const
{
    auto it = m_atomCounts.find(elementType);
    return (it != m_atomCounts.end()) ? it->second : 0;
}

void Sandbox2D::SetupDefaultSimulation()
{
    if (m_simulationSpace.IsRunning()) {
        m_simulationSpace.StopSimulation();
    }

    m_simulationSpace.ClearAllAtoms();

    m_simulationSpace.AddObject(Molecular::Atom("H", glm::vec2(0.0f, 0.0f)));
    m_simulationSpace.AddObject(Molecular::Atom("H", glm::vec2(0.3f, 0.0f)));


    m_simulationSpace.SaveInitialState();

    UpdateAtomCounts();
}

void Sandbox2D::ResetSimulation()
{
    if (m_simulationSpace.IsRunning()) {
        m_simulationSpace.StopSimulation();
    }

    m_simulationSpace.ResetSimulation();

    UpdateAtomCounts();
}