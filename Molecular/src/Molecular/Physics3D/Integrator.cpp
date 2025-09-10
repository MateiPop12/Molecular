#include "Integrator.h"

namespace Molecular
{
    Integrator::Integrator()
        : m_firstStep(true)
    {
    }

    void Integrator::VelocityVerlet(std::vector<PhysicsObject*>& objects, double deltaTime, ForceCalculator forceFunc)
    {
        const size_t numObjects = objects.size();

        // Resize acceleration storage if needed
        if (m_previousAccelerations.size() != numObjects)
        {
            m_previousAccelerations.resize(numObjects);
            m_firstStep = true;
        }

        // Calculate current accelerations
        std::vector<glm::dvec3> currentAccelerations(numObjects);
        for (size_t i = 0; i < numObjects; ++i)
        {
            currentAccelerations[i] = CalculateAcceleration(*objects[i], objects, forceFunc);
        }

        // For the first step, we need to handle it specially since we don't have previous accelerations
        if (m_firstStep)
        {
            // Simple Euler step for the first iteration
            for (size_t i = 0; i < numObjects; ++i)
            {
                PhysicsObject* obj = objects[i];

                // Update position: x(t+dt) = x(t) + v(t)*dt + 0.5*a(t)*dt^2
                glm::dvec3 newPosition = obj->GetPositionD() + obj->GetVelocityD() * deltaTime + 0.5 * currentAccelerations[i] * deltaTime * deltaTime;

                // Update velocity: v(t+dt) = v(t) + a(t)*dt
                glm::dvec3 newVelocity = obj->GetVelocityD() + currentAccelerations[i] * deltaTime;

                obj->SetPosition(newPosition);
                obj->SetVelocity(newVelocity);

                m_previousAccelerations[i] = currentAccelerations[i];
            }
            m_firstStep = false;
            return;
        }

        // Velocity-Verlet integration for the following steps
        for (size_t i = 0; i < numObjects; ++i)
        {
            PhysicsObject* obj = objects[i];

            // Update position: x(t+dt) = x(t) + v(t)*dt + 0.5*a(t)*dt^2
            glm::dvec3 newPosition = obj->GetPositionD() + obj->GetVelocityD() * deltaTime + 0.5 * currentAccelerations[i] * deltaTime * deltaTime;
            obj->SetPosition(newPosition);
        }

        // Calculate new accelerations at new positions
        std::vector<glm::dvec3> newAccelerations(numObjects);
        for (size_t i = 0; i < numObjects; ++i)
        {
            newAccelerations[i] = CalculateAcceleration(*objects[i], objects, forceFunc);
        }

        // Update velocities: v(t+dt) = v(t) + 0.5*(a(t) + a(t+dt))*dt
        for (size_t i = 0; i < numObjects; ++i)
        {
            PhysicsObject* obj = objects[i];
            glm::dvec3 newVelocity = obj->GetVelocityD() + 0.5 * (currentAccelerations[i] + newAccelerations[i]) * deltaTime;
            obj->SetVelocity(newVelocity);

            // Store current accelerations for the next step
            m_previousAccelerations[i] = newAccelerations[i];
        }
    }

    void Integrator::Reset()
    {
        m_previousAccelerations.clear();
        m_firstStep = true;
    }
}
