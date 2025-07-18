#pragma once

#include "Atom.h"
#include "BoundingBox.h"
#include "ForceCalculator.h"
#include "Integrator.h"
#include "Molecular/Core/Timestep.h"

#include <chrono>

namespace Molecular{

    class SimulationSpace {
    public:
        SimulationSpace();
        explicit SimulationSpace(IntegrationMethod method, double energyLossFactor = 0.9);

        // Object management
        void AddObject(const Atom& atom);
        void Update(Molecular::Timestep timeStep, const BoundingBox& boundingBox);

        // Simulation control
        void StartSimulation() { m_isRunning = true; }
        void StopSimulation() { m_isRunning = false; }
        void ResetSimulation();
        void ClearAllAtoms();
        void ResetToInitialPositions();
        void SaveInitialState();

        // Configuration
        void SetEnergyLossFactor(double energyLossFactor);
        void SetIntegrationMethod(IntegrationMethod method);
        void SetMaxForce(double maxForce);

        // Bond management
        void UpdateBonds();
        int GetTotalBondCount() const;
        std::vector<std::pair<size_t, size_t>> GetBondPairs() const;

        // Energy tracking
        void RecordEnergyData(double currentTime);
        void ExportEnergyDataToCSV(const std::string& filename = "") const;
        void ClearEnergyHistory() { m_energyHistory.clear(); m_timeHistory.clear(); }
        double CalculateTotalEnergy() const;

        // Getters
        bool IsRunning() const { return m_isRunning; }
        double GetEnergyLossFactor() const;
        IntegrationMethod GetIntegrationMethod() const;
        const std::vector<Atom>& GetObjects() const { return m_atoms; }
        std::vector<Atom>& GetObjectsMutable() { return m_atoms; }
        const std::vector<float>& GetEnergyHistory() const { return m_energyHistory; }
        const std::vector<double>& GetTimeHistory() const { return m_timeHistory; }

    private:
        // Core simulation components
        ForceCalculator m_forceCalculator;
        Integrator m_integrator;

        // Simulation state
        bool m_isRunning = false;

        // Atom storage
        std::vector<Atom> m_atoms;
        std::vector<Atom> m_initialAtoms;

        // Energy tracking
        std::vector<float> m_energyHistory;
        std::vector<double> m_timeHistory;

        // Configuration
        static constexpr size_t m_maxEnergyHistory = 1000000;
        static constexpr int m_energyRecordInterval = 5;

        // Internal counters
        mutable int m_recordCounter = 0;
        mutable double m_accumulatedTime = 0.0;
    };
}