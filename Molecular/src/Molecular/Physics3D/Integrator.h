#pragma once

#include "PhysicsObject.h"
#include "ForceCalculator3D.h"

namespace Molecular
{
    class Integrator
    {
    public:
        Integrator() = default;
        virtual ~Integrator() = default;

        // Advance all objects by one time-step using Velocity-Verlet integration.
        // deltaTime is in seconds; positions are in femtometres.
        void VelocityVerlet(std::vector<PhysicsObject*>& objects,
                            double deltaTime,
                            const ForceCalculator3D& forceCalc);

        void Reset();

    private:
        // Sum all pairwise forces on 'obj' from every other object, then
        // divide by mass to get acceleration (m/s²).
        [[nodiscard]] static glm::dvec3 CalculateAcceleration(
            const PhysicsObject&              obj,
            const std::vector<PhysicsObject*>& others,
            const ForceCalculator3D&             forceCalc);
    };
}