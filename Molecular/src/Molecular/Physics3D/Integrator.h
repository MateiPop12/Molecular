#pragma once

#include "PhysicsObject.h"
#include "ForceCalculator.h"

namespace Molecular
{
    class Integrator
    {
    public:
        Integrator();
        virtual ~Integrator() = default;

        void VelocityVerlet(std::vector<PhysicsObject*>& objects, double deltaTime,ForceCalculator forceFunc);
        void Reset();
    private:
        std::vector<glm::dvec3> m_previousAccelerations;
        bool m_firstStep;
    };
}