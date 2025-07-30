#pragma once

#include "Atom.h"

namespace Molecular
{
    class ForceCalculator
    {
    public:
        explicit ForceCalculator(double energyLossFactor = 0.9);

        [[nodiscard]] glm::dvec2 CalculateVanDerWaalsForce(const Atom& a, const Atom& b) const;
        [[nodiscard]] glm::dvec2 CalculateCoulombForce(const Atom& a, const Atom& b) const;
        [[nodiscard]] glm::dvec2 CalculateTotalForce(const Atom& atom, const std::vector<Atom>& allAtoms, size_t atomIndex) const;

        static double CalculateTotalEnergy(const std::vector<Atom>& atoms);
        static double CalculateKineticEnergy(const std::vector<Atom>& atoms);
        static double CalculatePotentialEnergy(const std::vector<Atom>& atoms);

        void HandleCollision(Atom& a, Atom& b) const;

        void SetEnergyLossFactor(const double factor) { m_energyLossFactor = factor; }
        void SetMaxForce(const double maxForce) { m_maxForce = maxForce; }

        [[nodiscard]] double GetEnergyLossFactor() const { return m_energyLossFactor; }
        [[nodiscard]] double GetMaxForce() const { return m_maxForce; }

    private:
        double m_energyLossFactor;
        double m_maxForce = 1e3;

        static double CalculateMinDistance(const Atom& a, const Atom& b);
        [[nodiscard]] glm::dvec2 ClampForce(const glm::dvec2& force) const;
    };
}