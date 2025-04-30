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
        void Update(Molecular::Timestep m_timeStep, BoundingBox boundingBox);

        void Euler(size_t i, double dt, BoundingBox boundingBox);
        void RungeKutta4(size_t i, double dt, BoundingBox boundingBox);
        void BoundingBoxCollision(glm::dvec2& position, glm::dvec2& velocity, const BoundingBox& boundingBox);

        void SetEnergyLossFactor(double energyLossFactor) { m_energyLossFactor = energyLossFactor; }
        void SetIntegrationMethod(IntegrationMethod method) { m_integrationMethod = method; }

        double GetEnergyLossFactor() const { return m_energyLossFactor;}
        double CalculateTotalEnergy();

        glm::dvec2 CalculateVanDerWaalsForce(const Atom& a, const Atom& b);
        glm::dvec2 CalculateCoulombForce(const Atom& a, const Atom& b);

        const std::vector<Atom>& GetObjects() const { return m_atoms; }
        const std::vector<float>& GetEnergyHistory() const { return m_energyHistory; }

        IntegrationMethod GetIntegrationMethod() const { return m_integrationMethod; }

    private:

        void Collision(Atom &a, Atom &b);
        IntegrationMethod m_integrationMethod = IntegrationMethod::RungeKutta4;

        double m_energyLossFactor = 0.9;

        std::vector<Atom> m_atoms;
        std::vector<float> m_energyHistory;

        const size_t m_maxEnergyHistory = 1000000;
    };
}
