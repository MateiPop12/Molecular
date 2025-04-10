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
        glm::dvec2 CalculateVanDerWaalsForce(const Atom& a, const Atom& b);
        void Update(Molecular::Timestep m_timeStep, BoundingBox boundingBox);
        void Euler(Molecular::Timestep m_timeStep, BoundingBox boundingBox);
        void RungeKutta4(Molecular::Timestep m_timeStep, BoundingBox boundingBox);
        void BoundingBoxCollision(glm::dvec2& position, glm::dvec2& velocity, const BoundingBox& boundingBox);
        double CalculateTotalEnergy();

        const std::vector<Atom>& GetObjects() const { return m_atoms; }
        IntegrationMethod GetIntegrationMethod() const { return m_integrationMethod; }
        void SetIntegrationMethod(IntegrationMethod method) { m_integrationMethod = method; }
        const std::vector<float>& GetEnergyHistory() const { return m_energyHistory; }
        double GetEnergyLossFactor() const { return m_energyLossFactor;}
        void SetEnergyLossFactor(double energyLossFactor) { m_energyLossFactor = energyLossFactor; }

    private:
        void Collision(Atom &a, Atom &b);

        IntegrationMethod m_integrationMethod = IntegrationMethod::RungeKutta4;

        std::vector<Atom> m_atoms;
        std::vector<float> m_energyHistory;

        double m_energyLossFactor = 0.9;

        const size_t m_maxEnergyHistory = 1000000;
    };
}
