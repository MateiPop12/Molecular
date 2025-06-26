#include "SimulationSpace.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/norm.hpp>
#include <Core/Log.h>
#include "Molecular.h"


//TODO::Refactor the Update and integration method so that the first atom loop is outside the integration method
namespace Molecular {

    void SimulationSpace::AddObject(const Atom &atom) {
        m_atoms.push_back(atom);
        if (!m_isRunning) {
            m_initialAtoms.push_back(atom);
        }
    }

    void SimulationSpace::Update(Molecular::Timestep m_timeStep, BoundingBox boundingBox) {
        if (!m_isRunning) {
            return;
        }

        double dt = m_timeStep.GetSeconds();
        static double accumulatedTime = 0.0;  // Track total simulation time
        accumulatedTime += dt;

        for (size_t i = 0; i < m_atoms.size(); ++i) {
            if (m_integrationMethod == IntegrationMethod::Euler) {
                Euler(i, dt, boundingBox);
            } else if (m_integrationMethod == IntegrationMethod::RungeKutta4) {
                RungeKutta4(i, dt, boundingBox);
            }
        }

        // Record energy data every few timesteps to avoid excessive data
        static int recordCounter = 0;
        const int recordInterval = 5;

        if (recordCounter++ % recordInterval == 0) {
            RecordEnergyData(accumulatedTime);
        }
    }

    void SimulationSpace::ResetSimulation() {
        StopSimulation();
        ResetToInitialPositions();
        m_energyHistory.clear();
        m_timeHistory.clear();  // Clear time history as well
        for (auto& atom : m_atoms) {
            atom.GetBondedAtoms().clear();
        }
    }

    void SimulationSpace::ClearAllAtoms() {
        StopSimulation();
        m_atoms.clear();
        m_initialAtoms.clear();
        m_energyHistory.clear();
    }

    void SimulationSpace::ResetToInitialPositions() {
        if (m_initialAtoms.size() == m_atoms.size()) {
            // Reset positions and velocities to initial state
            for (size_t i = 0; i < m_atoms.size(); ++i) {
                m_atoms[i].SetPosition(m_initialAtoms[i].GetPositionD());
                m_atoms[i].SetVelocity(m_initialAtoms[i].GetVelocityD());
                m_atoms[i].SetCharge(m_initialAtoms[i].GetCharge());
                // Clear bonds
                m_atoms[i].GetBondedAtoms().clear();
            }
        }
    }

    void SimulationSpace::SaveInitialState() {
        m_initialAtoms.clear();
        for (const auto& atom : m_atoms) {
            m_initialAtoms.push_back(atom);
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

    void SimulationSpace::Collision(Atom &a, Atom &b) {
        // Calculate the vector between the two atoms' positions
        glm::vec2 r = b.GetPosition() - a.GetPosition();
        double r_len = glm::length(r);

        // Hard limit for hydrogen atoms' collision (bond length)
        float minDistance;
        if (a.IsBondedTo(&b)) {
            minDistance = (a.GetCovalentBondLengthD() + b.GetCovalentBondLengthD()) * 0.5f;
        } else {
            minDistance = (a.GetVanDerWaalsRadiusD() + b.GetVanDerWaalsRadiusD()) * 0.9f;
        }

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

    //TODO::abstract the force calculation loop and optimize the calculation based on distance
    void SimulationSpace::Euler(size_t i, double dt, BoundingBox boundingBox) {

        glm::dvec2 totalForce(0.0);

        for (size_t j = 0; j < m_atoms.size(); ++j) {
            if (i != j) {
                totalForce += CalculateVanDerWaalsForce(m_atoms[i], m_atoms[j]);
                totalForce += CalculateCoulombForce(m_atoms[i],m_atoms[j]);
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

    void SimulationSpace::RungeKutta4(size_t i, double dt, BoundingBox boundingBox) {
        Atom& atom = m_atoms[i];

        glm::dvec2 position = atom.GetPositionD();
        glm::dvec2 velocity = atom.GetVelocityD();
        double mass = atom.GetMassD();

        auto ComputeAcceleration = [&](glm::dvec2 pos, glm::dvec2 vel) -> glm::dvec2 {
            glm::dvec2 totalForce(0.0);
            for (size_t j = 0; j < m_atoms.size(); ++j) {
                if (i != j) {
                    totalForce += CalculateVanDerWaalsForce(m_atoms[i], m_atoms[j]);
                    totalForce += CalculateCoulombForce(m_atoms[i],m_atoms[j]);
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

    glm::dvec2 SimulationSpace::CalculateCoulombForce(const Atom& a, const Atom& b) {
        constexpr double k_e = 8.9875517873681764e9; // Coulomb's constant in N·m²/C²
        constexpr double elementaryCharge = 1.602176634e-19; // Coulomb

        constexpr double softening = 1e-10;     // Softens singularity at r ≈ 0
        constexpr double dielectric = 1.0;      // Relative permittivity (1.0 = vacuum)

        glm::dvec2 r = b.GetPositionD() - a.GetPositionD();
        double r_len = glm::length(r) + 1e-10; // prevent division by zero

        if (r_len < 1e-10) return glm::dvec2(0.0);

        double softenedR = r_len + softening;

        double q1 = a.GetCharge() * elementaryCharge;
        double q2 = b.GetCharge() * elementaryCharge;

        double forceMagnitude = (k_e * q1 * q2) / (dielectric * softenedR * softenedR);

        return glm::normalize(r) * forceMagnitude;
    }

    void SimulationSpace::UpdateBonds() {
        if (!m_isRunning) return;

        // Check for new bond formation and bond breaking
        for (size_t i = 0; i < m_atoms.size(); ++i) {
            for (size_t j = i + 1; j < m_atoms.size(); ++j) {
                Atom& atomA = m_atoms[i];
                Atom& atomB = m_atoms[j];

                // Try to form new bonds
                if (!atomA.IsBondedTo(&atomB)) {
                    atomA.TryFormBond(&atomB);
                }
                    // Check if existing bonds should break
                else if (atomA.ShouldBreakBondWith(&atomB)) {
                    atomA.BreakBond(&atomB);
                }
            }
        }
    }

    int SimulationSpace::GetTotalBondCount() const {
        int totalBonds = 0;
        for (const auto& atom : m_atoms) {
            totalBonds += atom.GetBonds().size();
        }
        return totalBonds / 2; // Each bond is counted twice
    }

    std::vector<std::pair<size_t, size_t>> SimulationSpace::GetBondPairs() const {
        std::vector<std::pair<size_t, size_t>> bonds;

        for (size_t i = 0; i < m_atoms.size(); ++i) {
            for (const auto* bondedAtom : m_atoms[i].GetBonds()) {
                // Find the index of the bonded atom
                for (size_t j = i + 1; j < m_atoms.size(); ++j) {
                    if (&m_atoms[j] == bondedAtom) {
                        bonds.emplace_back(i, j);
                        break;
                    }
                }
            }
        }

        return bonds;
    }

    void SimulationSpace::RecordEnergyData(double currentTime) {
        if (!m_isRunning) return;

        double totalEnergy = CalculateTotalEnergy();

        // Only record if we haven't exceeded max history size
        if (m_energyHistory.size() < m_maxEnergyHistory) {
            m_energyHistory.push_back(static_cast<float>(totalEnergy));
            m_timeHistory.push_back(currentTime);
        } else {
            // Implement circular buffer behavior - remove oldest entries
            m_energyHistory.erase(m_energyHistory.begin());
            m_timeHistory.erase(m_timeHistory.begin());

            m_energyHistory.push_back(static_cast<float>(totalEnergy));
            m_timeHistory.push_back(currentTime);
        }
    }

    void SimulationSpace::ExportEnergyDataToCSV(const std::string &filename) const {
        std::string outputFilename = filename;

        MOL_CORE_INFO("STARTED EXPORTING");

        // Generate default filename if none provided
        if (outputFilename.empty()) {
            auto now = std::chrono::system_clock::now();
            auto time_t = std::chrono::system_clock::to_time_t(now);

            std::stringstream ss;

            ss << "energy_data_" << std::put_time(std::localtime(&time_t), "%d-%m-%Y_%H-%M-%S") << ".csv";

            outputFilename = ss.str();
        }

        std::ofstream file(outputFilename);
        if (!file.is_open()) {
            MOL_CORE_ERROR("FILE COULD NOT BE OPENED");
            return;
        }

        // Write CSV header
        file << "Time,Total_Energy\n";

        // Write energy data
        for (size_t i = 0; i < m_energyHistory.size() && i < m_timeHistory.size(); ++i) {
            file << std::fixed << std::setprecision(6)
                 << m_timeHistory[i] << ","
                 << m_energyHistory[i] << "\n";
        }

        MOL_CORE_INFO("FINISHED EXPORTING");

        file.close();
    }
}