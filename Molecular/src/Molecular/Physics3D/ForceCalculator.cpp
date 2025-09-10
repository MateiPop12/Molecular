#include "ForceCalculator.h"

namespace Molecular
{
    glm::dvec3 ForceCalculator::CalculateCoulombForce(const PhysicsObject& a, const PhysicsObject& b) const
    {
        const double k_e = 8.9875517923e9;          // Coulomb constant (N·m^2/C^2)
        const double e_charge = 1.602176634e-19;    // elementary charge (C)
    }
}
