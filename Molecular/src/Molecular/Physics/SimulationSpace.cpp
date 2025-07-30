#include "SimulationSpace.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <iomanip>
#include <gtx/norm.hpp>
#include <Core/Log.h>
#include "Molecular.h"


namespace Molecular {

    SimulationSpace::SimulationSpace()
        : m_forceCalculator(0.9), m_integrator(IntegrationMethod::RungeKutta4) {
    }

    SimulationSpace::SimulationSpace(IntegrationMethod method, double energyLossFactor)
        : m_forceCalculator(energyLossFactor), m_integrator(method) {
    }

    void SimulationSpace::AddObject(const Atom& atom) {
        m_atoms.push_back(atom);
        if (!m_isRunning) {
            m_initialAtoms.push_back(atom);
        }
    }

    void SimulationSpace::Update(Timestep timeStep, const BoundingBox& boundingBox) {
        if (!m_isRunning) {
            return;
        }

        const double dt = timeStep.GetSeconds();
        m_accumulatedTime += dt;

        // Update all atoms using the integrator
        for (size_t i = 0; i < m_atoms.size(); ++i) {
            m_integrator.Integrate(m_atoms[i], i, dt, m_atoms, boundingBox, m_forceCalculator);
        }

        // Record energy data periodically
        if (m_recordCounter++ % m_energyRecordInterval == 0) {
            RecordEnergyData(m_accumulatedTime);
        }
    }

    void SimulationSpace::ResetSimulation() {
        StopSimulation();
        ResetToInitialPositions();
        m_energyHistory.clear();
        m_timeHistory.clear();
        m_accumulatedTime = 0.0;
        m_recordCounter = 0;

        // Clear all bonds
        for (auto& atom : m_atoms) {
            atom.GetBondedAtoms().clear();
        }
    }

    void SimulationSpace::ClearAllAtoms() {
        StopSimulation();
        m_atoms.clear();
        m_initialAtoms.clear();
        m_energyHistory.clear();
        m_timeHistory.clear();
        m_accumulatedTime = 0.0;
        m_recordCounter = 0;
    }

    void SimulationSpace::ResetToInitialPositions() {
        if (m_initialAtoms.size() == m_atoms.size()) {
            for (size_t i = 0; i < m_atoms.size(); ++i) {
                m_atoms[i].SetPosition(m_initialAtoms[i].GetPositionD());
                m_atoms[i].SetVelocity(m_initialAtoms[i].GetVelocityD());
                m_atoms[i].SetCharge(m_initialAtoms[i].GetCharge());
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

    void SimulationSpace::SetEnergyLossFactor(double energyLossFactor) {
        m_forceCalculator.SetEnergyLossFactor(energyLossFactor);
    }

    void SimulationSpace::SetIntegrationMethod(IntegrationMethod method) {
        m_integrator.SetIntegrationMethod(method);
    }

    void SimulationSpace::SetMaxForce(double maxForce) {
        m_forceCalculator.SetMaxForce(maxForce);
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
            totalBonds += static_cast<int>(atom.GetBonds().size());
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

        const double totalEnergy = CalculateTotalEnergy();

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

    void SimulationSpace::ExportEnergyDataToCSV(const std::string& filename) const {
        std::string outputFilename = filename;

        MOL_CORE_INFO("STARTED EXPORTING");

        // Generate default filename if none provided
        if (outputFilename.empty()) {
            const auto now = std::chrono::system_clock::now();
            const auto time_t = std::chrono::system_clock::to_time_t(now);

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

    double SimulationSpace::CalculateTotalEnergy() const {
        return ForceCalculator::CalculateTotalEnergy(m_atoms);
    }

    double SimulationSpace::GetEnergyLossFactor() const {
        return m_forceCalculator.GetEnergyLossFactor();
    }

    IntegrationMethod SimulationSpace::GetIntegrationMethod() const {
        return m_integrator.GetIntegrationMethod();
    }
}