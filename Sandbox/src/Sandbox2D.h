#pragma once

#include <random>
#include "Molecular.h"

class Sandbox2D : public Molecular::Layer
{
public:
    Sandbox2D();
    virtual ~Sandbox2D() = default;

    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate(Molecular::Timestep ts) override;
    void OnImGuiRender() override;
    void OnEvent(Molecular::Event& e) override;
private:

    struct AtomSetupData {
        std::string elementType = "H";
        int count = 1;
    };

    // Atom management functions
    void AddRandomAtom(const std::string& elementType);
    void RemoveLastAtom(const std::string& elementType);

    void ResetSimulation();
    void SetupDefaultSimulation();

    glm::vec2 GenerateRandomPosition();
    bool IsPositionValid(const glm::vec2& position, float radius);
    void UpdateAtomCounts();
    int GetAtomCount(const std::string& elementType) const;


    std::vector<AtomSetupData> m_atomSetupList;
    std::vector<std::string> m_availableElements = {"H", "O", "C", "N"};

    // Atom count tracking
    std::unordered_map<std::string, int> m_atomCounts;

    // Simulation bounds
    float m_boundingBoxSize = 1.0f;

    // Random number generation
    std::mt19937 m_rng;
    std::uniform_real_distribution<float> m_positionDistribution;

    Molecular::OrthographicCameraController m_cameraController;
    Molecular::Ref<Molecular::Shader> m_shader;
    Molecular::Ref<Molecular::VertexArray> m_vertexArray;
    Molecular::Ref<Molecular::Texture2D> m_texture;

    Molecular::SimulationSpace m_simulationSpace;
    glm::vec3 m_objectColor = { 0.5f, 0.0f, 0.0f };
};
