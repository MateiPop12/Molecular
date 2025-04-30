#include "SimulationSpace.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/norm.hpp>

namespace Molecular{

    void SimulationSpace::AddObject(const Atom &atom) {
        m_atoms.push_back(atom);
    }

    glm::dvec2 SimulationSpace::CalculateVanDerWaalsForce(const Atom &a, const Atom &b) {

        double epsilon = (a.GetEpsilonD() + b.GetEpsilonD()) / 2.0;
        double sigma = (a.GetSigmaD() + b.GetSigmaD()) / 2.0;
        //double softRepulsionThreshold = (a.GetVanDerWaalsRadiusD() + b.GetVanDerWaalsRadiusD()) / 2.0;

        glm::dvec2 r = b.GetPositionD() - a.GetPositionD();
        double softening = 1e-2;
        double r_len = glm::length(r) + softening;

        // Avoid division by zero if atoms are too close
        if (r_len < 1e-10) return glm::dvec2(0.0);

        // Lennard-Jones potential F = 48 * epsilon * ( (sigma/r)^12 - 0.5 * (sigma/r)^6 ) * r_hat
        double r6 = glm::pow(sigma / r_len, 6);
        double r12 = r6 * r6;
        double forceMag = 48.0 * epsilon * (r12 - 0.5 * r6) / (r_len * r_len);

        const double maxForce = 1e3;
        if (std::abs(forceMag) > maxForce) {
            forceMag = glm::clamp(forceMag, -maxForce, maxForce);
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
        double dt = m_timeStep.GetSeconds();

        for (size_t i = 0; i < m_atoms.size(); ++i) {
            glm::dvec2 totalForce(0.0);

            for (size_t j = 0; j < m_atoms.size(); ++j) {
                if (i != j) {
                    totalForce += CalculateVanDerWaalsForce(m_atoms[i], m_atoms[j]);
                    Collision(m_atoms[i], m_atoms[j]);
                }
            }

            glm::dvec2 acceleration = totalForce / m_atoms[i].GetMassD();
            glm::dvec2 newVelocity = m_atoms[i].GetVelocityD() + acceleration * dt;
            glm::dvec2 newPosition = m_atoms[i].GetPositionD() + newVelocity * dt;

            BoundingBoxCollision(newPosition, newVelocity, boundingBox);

            m_atoms[i].SetVelocity(newVelocity);
            m_atoms[i].SetPosition(newPosition);
        }
    }

    void SimulationSpace::RungeKutta4(Molecular::Timestep m_timeStep, BoundingBox boundingBox) {
        double dt = m_timeStep.GetSeconds();

        for (size_t i = 0; i < m_atoms.size(); ++i) {
            Atom& atom = m_atoms[i];

            glm::dvec2 position = atom.GetPositionD();
            glm::dvec2 velocity = atom.GetVelocityD();
            double mass = atom.GetMassD();

            auto ComputeAcceleration = [&](glm::dvec2 pos, glm::dvec2 vel) -> glm::dvec2 {
                glm::dvec2 totalForce(0.0);
                for (size_t j = 0; j < m_atoms.size(); ++j) {
                    if (i != j) {
                        totalForce += CalculateVanDerWaalsForce(m_atoms[i], m_atoms[j]);
                        Collision(m_atoms[i], m_atoms[j]);
                    }
                }
                return totalForce / mass;
            };

            // RungeKutta4 Steps
            glm::dvec2 k1v = ComputeAcceleration(position, velocity) * dt;
            glm::dvec2 k1x = velocity * dt;

            glm::dvec2 k2v = ComputeAcceleration(position + k1x * 0.5, velocity + k1v * 0.5) * dt;
            glm::dvec2 k2x = (velocity + k1v * 0.5) * dt;

            glm::dvec2 k3v = ComputeAcceleration(position + k2x * 0.5, velocity + k2v * 0.5) * dt;
            glm::dvec2 k3x = (velocity + k2v * 0.5) * dt;

            glm::dvec2 k4v = ComputeAcceleration(position + k3x, velocity + k3v) * dt;
            glm::dvec2 k4x = (velocity + k3v) * dt;

            // Final updates
            glm::dvec2 newVelocity = velocity + (k1v + 2.0 * k2v + 2.0 * k3v + k4v) / 6.0;
            glm::dvec2 newPosition = position + (k1x + 2.0 * k2x + 2.0 * k3x + k4x) / 6.0;

            BoundingBoxCollision(newPosition, newVelocity, boundingBox);

            atom.SetVelocity(newVelocity);
            atom.SetPosition(newPosition);
        }
    }

    void SimulationSpace::BoundingBoxCollision(glm::dvec2 &position, glm::dvec2 &velocity, const BoundingBox &boundingBox) {
        if (!boundingBox.IsInside(position)) {
            if (position.x < boundingBox.GetMinPoint().x || position.x > boundingBox.GetMaxPoint().x) {
                velocity.x *= -1.0;
            }
            if (position.y < boundingBox.GetMinPoint().y || position.y > boundingBox.GetMaxPoint().y) {
                velocity.y *= -1.0;
            }

            position = glm::clamp(position, boundingBox.GetMinPoint(), boundingBox.GetMaxPoint());
        }
    }

    double SimulationSpace::CalculateTotalEnergy() {
        double totalKineticEnergy = 0.0;
        double totalPotentialEnergy = 0.0;

        // Kinetic Energy Calculation
        for (const auto& atom : m_atoms) {
            double speedSquared = glm::length2(atom.GetVelocityD()); // v^2
            totalKineticEnergy += 0.5 * atom.GetMassD() * speedSquared;
        }

        // Potential Energy Calculation (Pairwise)
        for (size_t i = 0; i < m_atoms.size(); ++i) {
            for (size_t j = i + 1; j < m_atoms.size(); ++j) {
                const Atom& a = m_atoms[i];
                const Atom& b = m_atoms[j];

                double epsilon = (a.GetEpsilonD() + b.GetEpsilonD()) / 2.0;
                double sigma = (a.GetSigmaD() + b.GetSigmaD()) / 2.0;

                glm::dvec2 r = b.GetPositionD() - a.GetPositionD();
                double r_len = glm::length(r);

                if (r_len > 1e-10) { // Avoid division by zero
                    double r6 = glm::pow(sigma / r_len, 6);
                    double r12 = r6 * r6;
                    totalPotentialEnergy += 4.0 * epsilon * (r12 - r6);
                }
            }
        }

        return abs(totalKineticEnergy + totalPotentialEnergy);
    }

    void SimulationSpace::Collision(Atom &a, Atom &b) {
        // Calculate the vector between the two atoms' positions
        glm::vec2 r = b.GetPosition() - a.GetPosition();
        double r_len = glm::length(r);

        // Hard limit for hydrogen atoms' collision (bond length)
        float minDistance = (a.GetVanDerWaalsRadiusD() + b.GetVanDerWaalsRadiusD()) * 0.9;

        if (r_len < minDistance) {
            // Calculate the normal vector between atoms (the line connecting their centers)
            glm::dvec2 normal = glm::normalize(r);

            // Reflect the velocities based on the normal vector
            glm::dvec2 velocityA = a.GetVelocityD();
            glm::dvec2 velocityB = b.GetVelocityD();

            // Reflection formula: v' = v - 2 * (v . n) * n
            double dotProductA = glm::dot(velocityA, normal);
            double dotProductB = glm::dot(velocityB, normal);

            glm::dvec2 reflectedVelocityA = velocityA - 2.0 * dotProductA * normal;
            glm::dvec2 reflectedVelocityB = velocityB - 2.0 * dotProductB * normal;

            // Apply small energy loss (multiplying velocity by a factor less than 1)
            reflectedVelocityA *= m_energyLossFactor;
            reflectedVelocityB *= m_energyLossFactor;

            // Adjust positions to prevent overlap
            glm::dvec2 displacement = normal * (minDistance - r_len) * 0.5;
            a.SetPosition(a.GetPositionD() - displacement);
            b.SetPosition(b.GetPositionD() + displacement);

            // Set the new velocities for the atoms
            a.SetVelocity(reflectedVelocityA);
            b.SetVelocity(reflectedVelocityB);
        }
    }
}