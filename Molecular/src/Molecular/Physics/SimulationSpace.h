#pragma once

#include "Atom.h"
#include "BoundingBox.h"
#include "Molecular/Core/Timestep.h"

namespace Molecular{

    enum class IntegrationMethod {
        Euler,
        RungeKutta4
    };

    class SimulationSpace {
    public:
        void AddObject(const Atom& atom);
        glm::vec2 CalculateVanDerWaalsForce(const Atom& a, const Atom& b);
        void Update(Molecular::Timestep m_timeStep, BoundingBox boundingBox);
        void Euler(Molecular::Timestep m_timeStep, BoundingBox boundingBox);
        void RungeKutta4(Molecular::Timestep m_timeStep, BoundingBox boundingBox);
        void BoundingBoxCollision(glm::vec2& position, glm::vec2& velocity, const BoundingBox& boundingBox);
        float CalculateTotalEnergy();
        void Collision(Atom &a, Atom &b);

        const std::vector<Atom>& GetObjects() const { return m_atoms; }
        IntegrationMethod GetIntegrationMethod() const { return m_integrationMethod; }
        void SetIntegrationMethod(IntegrationMethod method) { m_integrationMethod = method; }
        const std::vector<float>& GetEnergyHistory() const { return m_energyHistory; }

    private:
        IntegrationMethod m_integrationMethod = IntegrationMethod::RungeKutta4;

        std::vector<Atom> m_atoms;
        std::vector<float> m_energyHistory;

        const size_t m_maxEnergyHistory = 10000;
    };
}
