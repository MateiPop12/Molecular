#pragma once

#include "PhysicsObject.h"

namespace Molecular
{
    class ForceCalculator3D
    {
    public:
        // epsilon_fm: softening length in femtometres (fm).
        // Prevents the Coulomb force from diverging when two particles
        // occupy nearly the same position. A value of ~0.1 fm is a
        // reasonable default for subatomic scales.
        explicit ForceCalculator3D(double epsilon_fm = 0.1);

        // Coulomb electrostatic force on 'a' due to 'b' (Newtons).
        // Positions are expected in femtometres (fm = 1e-15 m).
        [[nodiscard]] glm::dvec3 CalculateCoulombForce(const PhysicsObject& a,
                                                       const PhysicsObject& b) const;

        // Yukawa approximation of the residual nuclear strong force (Newtons).
        // Only significant at ranges < ~2–3 fm.
        // Positions are expected in femtometres.
        [[nodiscard]] glm::dvec3 CalculateStrongForce(const PhysicsObject& a,
                                                      const PhysicsObject& b) const;

        // Net force on 'a' from 'b': Coulomb + strong (Newtons).
        [[nodiscard]] glm::dvec3 CalculateTotalForce(const PhysicsObject& a,
                                                     const PhysicsObject& b) const;

    private:
        double m_epsilon_m; // softening length converted to metres
    };
}