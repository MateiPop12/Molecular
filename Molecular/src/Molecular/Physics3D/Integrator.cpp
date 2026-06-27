#include "Integrator.h"

namespace Molecular
{
    // -----------------------------------------------------------------------
    // CalculateAcceleration
    // Sums the total pairwise force on 'obj' from every other object in
    // 'others' (skipping self), then returns F/m (m/s²).
    // -----------------------------------------------------------------------
    glm::dvec3 Integrator::CalculateAcceleration(const PhysicsObject&               obj,
                                                  const std::vector<PhysicsObject*>& others,
                                                  const ForceCalculator3D&             forceCalc)
    {
        glm::dvec3 totalForce(0.0);

        for (const PhysicsObject* other : others)
        {
            if (other == &obj)
                continue;

            totalForce += forceCalc.CalculateTotalForce(obj, *other);
        }

        return totalForce / obj.GetMass(); // a = F / m  (m/s²)
    }

    // -----------------------------------------------------------------------
    // VelocityVerlet
    //
    // Standard two-stage Velocity-Verlet:
    //
    //   1. Compute a(t)  from current positions.
    //   2. Update positions:  x(t+dt) = x(t) + v(t)*dt + ½*a(t)*dt²
    //   3. Compute a(t+dt) from new positions.
    //   4. Update velocities: v(t+dt) = v(t) + ½*(a(t) + a(t+dt))*dt
    //
    // This is time-reversible and second-order accurate with no special
    // first-step handling required.
    // -----------------------------------------------------------------------
    void Integrator::VelocityVerlet(std::vector<PhysicsObject*>& objects,
                                    double deltaTime,
                                    const ForceCalculator3D& forceCalc)
    {
        const size_t n = objects.size();

        // --- Stage 1: accelerations at current positions ---
        std::vector<glm::dvec3> acc(n);
        for (size_t i = 0; i < n; ++i)
            acc[i] = CalculateAcceleration(*objects[i], objects, forceCalc);

        // --- Stage 2: update positions ---
        for (size_t i = 0; i < n; ++i)
        {
            const glm::dvec3 newPos = objects[i]->GetPositionD()
                                    + objects[i]->GetVelocityD() * deltaTime
                                    + 0.5 * acc[i] * (deltaTime * deltaTime);
            objects[i]->SetPosition(newPos);
        }

        // --- Stage 3: accelerations at new positions ---
        std::vector<glm::dvec3> accNew(n);
        for (size_t i = 0; i < n; ++i)
            accNew[i] = CalculateAcceleration(*objects[i], objects, forceCalc);

        // --- Stage 4: update velocities with averaged acceleration ---
        for (size_t i = 0; i < n; ++i)
        {
            const glm::dvec3 newVel = objects[i]->GetVelocityD()
                                    + 0.5 * (acc[i] + accNew[i]) * deltaTime;
            objects[i]->SetVelocity(newVel);
        }
    }

    void Integrator::Reset()
    {
        // Nothing to reset in the stateless formulation.
        // Kept for API compatibility.
    }
}
