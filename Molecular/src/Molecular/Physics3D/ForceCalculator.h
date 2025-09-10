#pragma once

#include "PhysicsObject.h"

namespace Molecular
{
    class ForceCalculator
    {
    public:
        ForceCalculator();

        [[nodiscard]] glm::dvec3 CalculateCoulombForce(const PhysicsObject& a, const PhysicsObject& b) const;
    };
}