#include "ForceCalculator.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "gtx/norm.hpp"

namespace Molecular
{
    ForceCalculator::ForceCalculator(const double energyLossFactor)
        : m_energyLossFactor(energyLossFactor) {
    }

    glm::dvec2 ForceCalculator::CalculateVanDerWaalsForce(const Atom& a, const Atom& b) const
    {
        const double epsilon = (a.GetEpsilonD() + b.GetEpsilonD()) / 2.0;
        const double sigma = (a.GetSigmaD() + b.GetSigmaD()) / 2.0;

        const glm::dvec2 r = b.GetPositionD() - a.GetPositionD();
        constexpr double softening = 1e-2;
        const double r_len = glm::length(r) + softening;

        // Avoid division by zero if atoms are too close
        if (r_len < 1e-10) return glm::dvec2(0.0);

        // Lennard-Jones potential F = 48 * epsilon * ( (sigma/r)^12 - 0.5 * (sigma/r)^6 ) * r_hat
        const double r6 = glm::pow(sigma / r_len, 6);
        const double r12 = r6 * r6;
        double forceMag = 48.0 * epsilon * (r12 - 0.5 * r6) / (r_len * r_len);

        // Clamp force to prevent numerical instability
        forceMag = glm::clamp(forceMag, -m_maxForce, m_maxForce);

        return glm::normalize(r) * forceMag;
    }

    glm::dvec2 ForceCalculator::CalculateCoulombForce(const Atom& a, const Atom& b) const
    {
        constexpr double k_e = 8.9875517873681764e9; // Coulomb's constant in N·m²/C²
        constexpr double elementaryCharge = 1.602176634e-19; // Coulomb
        constexpr double softening = 1e-10;     // Softens singularity at r ≈ 0
        constexpr double dielectric = 1.0;      // Relative permittivity (1.0 = vacuum)

        const glm::dvec2 r = b.GetPositionD() - a.GetPositionD();
        const double r_len = glm::length(r);

        if (r_len < 1e-10) return glm::dvec2(0.0);

        const double softenedR = r_len + softening;
        const double q1 = a.GetCharge() * elementaryCharge;
        const double q2 = b.GetCharge() * elementaryCharge;

        double forceMagnitude = (k_e * q1 * q2) / (dielectric * softenedR * softenedR);

        // Clamp force to prevent numerical instability
        forceMagnitude = glm::clamp(forceMagnitude, -m_maxForce, m_maxForce);

        return glm::normalize(r) * forceMagnitude;
    }

    glm::dvec2 ForceCalculator::CalculateTotalForce(const Atom& atom, const std::vector<Atom>& allAtoms, const size_t atomIndex) const
    {
        glm::dvec2 totalForce(0.0);

        // Calculate pairwise forces
        for (size_t j = 0; j < allAtoms.size(); ++j) {
            if (atomIndex != j) {
                totalForce += CalculateVanDerWaalsForce(atom, allAtoms[j]);
                totalForce += CalculateCoulombForce(atom, allAtoms[j]);
            }
        }

        return ClampForce(totalForce);
    }

    double ForceCalculator::CalculateTotalEnergy(const std::vector<Atom>& atoms) {
        return CalculateKineticEnergy(atoms) + CalculatePotentialEnergy(atoms);
    }

    double ForceCalculator::CalculateKineticEnergy(const std::vector<Atom>& atoms) {
        double totalKineticEnergy = 0.0;

        for (const auto& atom : atoms) {
            const double speedSquared = glm::length2(atom.GetVelocityD()); // v^2
            totalKineticEnergy += 0.5 * atom.GetMassD() * speedSquared;
        }

        return totalKineticEnergy;
    }

    double ForceCalculator::CalculatePotentialEnergy(const std::vector<Atom>& atoms) {
        double totalPotentialEnergy = 0.0;

        // Potential Energy Calculation (Pairwise)
        for (size_t i = 0; i < atoms.size(); ++i) {
            for (size_t j = i + 1; j < atoms.size(); ++j) {
                const Atom& a = atoms[i];
                const Atom& b = atoms[j];

                const double epsilon = (a.GetEpsilonD() + b.GetEpsilonD()) / 2.0;
                const double sigma = (a.GetSigmaD() + b.GetSigmaD()) / 2.0;

                glm::dvec2 r = b.GetPositionD() - a.GetPositionD();

                if (const double r_len = glm::length(r); r_len > 1e-10) { // Avoid division by zero
                    const double r6 = glm::pow(sigma / r_len, 6);
                    const double r12 = r6 * r6;
                    totalPotentialEnergy += 4.0 * epsilon * (r12 - r6);
                }
            }
        }

        return totalPotentialEnergy;
    }

    void ForceCalculator::HandleCollision(Atom& a, Atom& b) const
    {
        // Calculate the vector between the two atoms' positions
        const glm::dvec2 r = b.GetPositionD() - a.GetPositionD();
        const double r_len = glm::length(r);

        // Calculate minimum distance based on bond status

        if (const double minDistance = CalculateMinDistance(a, b); r_len < minDistance && r_len > 1e-10) {
            // Calculate the normal vector between atoms
            const glm::dvec2 normal = glm::normalize(r);

            // Get current velocities
            const glm::dvec2 velocityA = a.GetVelocityD();
            const glm::dvec2 velocityB = b.GetVelocityD();

            // Reflection formula: v' = v - 2 * (v . n) * n
            const double dotProductA = glm::dot(velocityA, normal);
            const double dotProductB = glm::dot(velocityB, normal);

            glm::dvec2 reflectedVelocityA = velocityA - 2.0 * dotProductA * normal;
            glm::dvec2 reflectedVelocityB = velocityB - 2.0 * dotProductB * normal;

            // Apply energy loss
            reflectedVelocityA *= m_energyLossFactor;
            reflectedVelocityB *= m_energyLossFactor;

            // Adjust positions to prevent overlap
            const glm::dvec2 displacement = normal * (minDistance - r_len) * 0.5;
            a.SetPosition(a.GetPositionD() - displacement);
            b.SetPosition(b.GetPositionD() + displacement);

            // Set the new velocities
            a.SetVelocity(reflectedVelocityA);
            b.SetVelocity(reflectedVelocityB);
        }
    }

    double ForceCalculator::CalculateMinDistance(const Atom& a, const Atom& b) {
        if (a.IsBondedTo(&b)) {
            return (a.GetCovalentBondLengthD() + b.GetCovalentBondLengthD()) * 0.5;
        } else {
            return (a.GetVanDerWaalsRadiusD() + b.GetVanDerWaalsRadiusD()) * 0.9;
        }
    }

    glm::dvec2 ForceCalculator::ClampForce(const glm::dvec2& force) const
    {
        if (const double magnitude = glm::length(force); magnitude > m_maxForce) {
            return glm::normalize(force) * m_maxForce;
        }
        return force;
    }
}
