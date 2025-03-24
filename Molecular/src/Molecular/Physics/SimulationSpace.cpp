#include "SimulationSpace.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/norm.hpp>
#include <Core/Log.h>

namespace Molecular{

    void SimulationSpace::AddObject(const Atom &atom) {
        m_atoms.push_back(atom);
    }

    glm::vec2 SimulationSpace::CalculateVanDerWaalsForce(const Atom &a, const Atom &b) {

        float epsilon = (a.GetEpsilon() + b.GetEpsilon()) / 2.0f;  // Depth of the potential well for atomic hydrogen (in eV)
        float sigma = (a.GetSigma() + b.GetSigma()) / 2.0f;     // Distance at which the potential is zero for atomic hydrogen (in Ångströms)
        float softRepulsionThreshold = (a.GetVanDerWaalsRadius() + b.GetVanDerWaalsRadius()) / 2.0f;

        glm::vec2 r = b.GetPosition() - a.GetPosition();
        float r_len = glm::length(r);

        // Avoid division by zero, if atoms are too close
        if (r_len < 1e-5f) return glm::vec2(0.0f, 0.0f);

        // Lennard-Jones potential F = 48 * epsilon * ( (sigma/r)^12 - 0.5 * (sigma/r)^6 ) * r_hat
        float r6 = glm::pow(sigma / r_len, 6);
        float r12 = r6 * r6;
        float forceMag = 48.0f * epsilon * (r12 - 0.5f * r6) / (r_len * r_len);

        // Soft repulsion for very close atoms
        if (r_len < softRepulsionThreshold) {
            float repulsionStrength = 50.0f; // Arbitrary strong repulsion factor
            forceMag += repulsionStrength * (1.0f / r_len - 1.0f / softRepulsionThreshold);
        }

        return glm::normalize(r) * forceMag;
    }

    void SimulationSpace::Update(Molecular::Timestep m_timeStep, BoundingBox boundingBox) {
        if (m_integrationMethod == IntegrationMethod::Euler) {
            Euler(m_timeStep, boundingBox);
        } else if (m_integrationMethod == IntegrationMethod::RungeKutta4) {
            RungeKutta4(m_timeStep, boundingBox);
        }

        float energy = CalculateTotalEnergy();
        if (m_energyHistory.size() >= m_maxEnergyHistory) {
            m_energyHistory.erase(m_energyHistory.begin()); // Remove oldest entry
        }
        m_energyHistory.push_back(energy);
    }

    void SimulationSpace::Euler(Molecular::Timestep m_timeStep, BoundingBox boundingBox) {
        float dt = m_timeStep.GetSeconds();

        for (size_t i = 0; i < m_atoms.size(); ++i) {
            glm::vec2 totalForce(0.0f, 0.0f);

            for (size_t j = 0; j < m_atoms.size(); ++j) {
                if (i != j) {
                    totalForce += CalculateVanDerWaalsForce(m_atoms[i], m_atoms[j]);
                }
            }

            glm::vec2 acceleration = totalForce / m_atoms[i].GetMass();
            glm::vec2 newVelocity = m_atoms[i].GetVelocity() + acceleration * dt;
            glm::vec2 newPosition = m_atoms[i].GetPosition() + newVelocity * dt;

            BoundingBoxCollision(newPosition, newVelocity, boundingBox);

            m_atoms[i].SetVelocity(newVelocity);
            m_atoms[i].SetPosition(newPosition);
        }
    }

    void SimulationSpace::RungeKutta4(Molecular::Timestep m_timeStep, BoundingBox boundingBox) {
        float dt = m_timeStep.GetSeconds();

        for (size_t i = 0; i < m_atoms.size(); ++i) {
            Atom& atom = m_atoms[i];

            glm::vec2 position = atom.GetPosition();
            glm::vec2 velocity = atom.GetVelocity();
            float mass = atom.GetMass();

            auto ComputeAcceleration = [&](glm::vec2 pos, glm::vec2 vel) -> glm::vec2 {
                glm::vec2 totalForce(0.0f, 0.0f);
                for (size_t j = 0; j < m_atoms.size(); ++j) {
                    if (i != j) {
                        totalForce += CalculateVanDerWaalsForce(m_atoms[i], m_atoms[j]);
                    }
                }
                return totalForce / mass;
            };

            // RungeKutta4 Steps
            glm::vec2 k1v = ComputeAcceleration(position, velocity) * dt;
            glm::vec2 k1x = velocity * dt;

            glm::vec2 k2v = ComputeAcceleration(position + k1x * 0.5f, velocity + k1v * 0.5f) * dt;
            glm::vec2 k2x = (velocity + k1v * 0.5f) * dt;

            glm::vec2 k3v = ComputeAcceleration(position + k2x * 0.5f, velocity + k2v * 0.5f) * dt;
            glm::vec2 k3x = (velocity + k2v * 0.5f) * dt;

            glm::vec2 k4v = ComputeAcceleration(position + k3x, velocity + k3v) * dt;
            glm::vec2 k4x = (velocity + k3v) * dt;

            // Final updates
            glm::vec2 newVelocity = velocity + (k1v + 2.0f * k2v + 2.0f * k3v + k4v) / 6.0f;
            glm::vec2 newPosition = position + (k1x + 2.0f * k2x + 2.0f * k3x + k4x) / 6.0f;

            BoundingBoxCollision(newPosition, newVelocity, boundingBox);

            atom.SetVelocity(newVelocity);
            atom.SetPosition(newPosition);
        }
    }

    void SimulationSpace::BoundingBoxCollision(glm::vec2 &position, glm::vec2 &velocity, const BoundingBox &boundingBox) {
        if (!boundingBox.IsInside(position)) {
            if (position.x < boundingBox.GetMinPoint().x || position.x > boundingBox.GetMaxPoint().x) {
                velocity.x *= -1.0f;
            }
            if (position.y < boundingBox.GetMinPoint().y || position.y > boundingBox.GetMaxPoint().y) {
                velocity.y *= -1.0f;
            }

            position = glm::clamp(position, boundingBox.GetMinPoint(), boundingBox.GetMaxPoint());
        }
    }

    float SimulationSpace::CalculateTotalEnergy() {
        float totalKineticEnergy = 0.0f;
        float totalPotentialEnergy = 0.0f;

        // Kinetic Energy Calculation
        for (const auto& atom : m_atoms) {
            float speedSquared = glm::length2(atom.GetVelocity()); // v^2
            totalKineticEnergy += 0.5f * atom.GetMass() * speedSquared;
        }

        // Potential Energy Calculation (Pairwise)
        for (size_t i = 0; i < m_atoms.size(); ++i) {
            for (size_t j = i + 1; j < m_atoms.size(); ++j) {
                const Atom& a = m_atoms[i];
                const Atom& b = m_atoms[j];

                float epsilon = (a.GetEpsilon() + b.GetEpsilon()) / 2.0f;
                float sigma = (a.GetSigma() + b.GetSigma()) / 2.0f;

                glm::vec2 r = b.GetPosition() - a.GetPosition();
                float r_len = glm::length(r);

                if (r_len > 1e-5f) { // Avoid division by zero
                    float r6 = glm::pow(sigma / r_len, 6);
                    float r12 = r6 * r6;
                    totalPotentialEnergy += 4.0f * epsilon * (r12 - r6);
                }
            }
        }

        return totalKineticEnergy + totalPotentialEnergy;
    }
}