#include "Integrator.h"

#include <algorithm>

namespace Molecular
{
    Integrator::Integrator(const IntegrationMethod method)
        : m_method(method) {
    }

    void Integrator::Integrate(Atom& atom, const size_t atomIndex, const double dt,
                              const std::vector<Atom>& allAtoms,
                              const BoundingBox& boundingBox,
                              ForceCalculator& forceCalc) const
    {
        double actualDt = dt;

        // Use adaptive time stepping if enabled
        if (m_useAdaptiveTimeStep) {
            actualDt = AdaptiveTimeStep(atom, atomIndex, dt, allAtoms, forceCalc);
        }

        switch (m_method) {
        case IntegrationMethod::Euler:
            EulerStep(atom, atomIndex, actualDt, allAtoms, boundingBox, forceCalc);
            break;
        case IntegrationMethod::RungeKutta4:
            RungeKutta4Step(atom, atomIndex, actualDt, allAtoms, boundingBox, forceCalc);
            break;
        case IntegrationMethod::LeapFrog:
            LeapFrogStep(atom, atomIndex, actualDt, allAtoms, boundingBox, forceCalc);
            break;
        default:
            VelocityVerletStep(atom, atomIndex, actualDt, allAtoms, boundingBox, forceCalc);
            break;
        }
    }

    void Integrator::InitializeVerlet(std::vector<Atom>& atoms, const double dt,
                                     const BoundingBox& boundingBox,
                                     const ForceCalculator& forceCalc)
    {
        // Initialize previous positions for Verlet method
        for (size_t i = 0; i < atoms.size(); ++i) {
            Atom& atom = atoms[i];

            // Calculate initial acceleration
            const glm::dvec2 initialAcceleration = ComputeAcceleration(atom, i,
                                                                      atom.GetPositionD(),
                                                                      atom.GetVelocityD(),
                                                                      atoms, forceCalc);

            // Set previous position using: x(t-dt) = x(t) - v(t)*dt + 0.5*a(t)*dt^2
            const glm::dvec2 previousPosition = atom.GetPositionD() - atom.GetVelocityD() * dt +
                                               0.5 * initialAcceleration * dt * dt;

            atom.SetPreviousPosition(previousPosition);
        }
    }

    void Integrator::EulerStep(Atom& atom, const size_t atomIndex, const double dt,
                              const std::vector<Atom>& allAtoms,
                              const BoundingBox& boundingBox,
                              const ForceCalculator& forceCalc)
    {
        // Handle collisions first
        HandleCollisions(atom, atomIndex, allAtoms, forceCalc);

        // Calculate total force and acceleration
        const glm::dvec2 totalForce = forceCalc.CalculateTotalForce(atom, allAtoms, atomIndex);
        const glm::dvec2 acceleration = totalForce / atom.GetMassD();

        // Euler integration: v(t+dt) = v(t) + a(t)*dt, x(t+dt) = x(t) + v(t)*dt
        glm::dvec2 newVelocity = atom.GetVelocityD() + acceleration * dt;
        glm::dvec2 newPosition = atom.GetPositionD() + atom.GetVelocityD() * dt;

        // Handle boundary collisions
        HandleBoundaryCollision(newPosition, newVelocity, boundingBox);

        // Update atom state
        atom.SetVelocity(newVelocity);
        atom.SetPosition(newPosition);
    }


    void Integrator::RungeKutta4Step(Atom& atom, const size_t atomIndex, const double dt,
                                    const std::vector<Atom>& allAtoms,
                                    const BoundingBox& boundingBox,
                                    ForceCalculator& forceCalc)
    {
        // Store initial state
        const glm::dvec2 initialPosition = atom.GetPositionD();
        const glm::dvec2 initialVelocity = atom.GetVelocityD();

        // Handle collisions first
        HandleCollisions(atom, atomIndex, allAtoms, forceCalc);

        // RK4 integration steps
        // k1: derivatives at t
        const glm::dvec2 k1v = ComputeAcceleration(atom, atomIndex, initialPosition, initialVelocity, allAtoms, forceCalc);
        const glm::dvec2 k1x = initialVelocity;

        // k2: derivatives at t + dt/2
        const glm::dvec2 k2v = ComputeAcceleration(atom, atomIndex,
                                                  initialPosition + k1x * dt * 0.5,
                                                  initialVelocity + k1v * dt * 0.5,
                                                  allAtoms, forceCalc);
        const glm::dvec2 k2x = initialVelocity + k1v * dt * 0.5;

        // k3: derivatives at t + dt/2 (using k2)
        const glm::dvec2 k3v = ComputeAcceleration(atom, atomIndex,
                                                  initialPosition + k2x * dt * 0.5,
                                                  initialVelocity + k2v * dt * 0.5,
                                                  allAtoms, forceCalc);
        const glm::dvec2 k3x = initialVelocity + k2v * dt * 0.5;

        // k4: derivatives at t + dt
        const glm::dvec2 k4v = ComputeAcceleration(atom, atomIndex,
                                                  initialPosition + k3x * dt,
                                                  initialVelocity + k3v * dt,
                                                  allAtoms, forceCalc);
        const glm::dvec2 k4x = initialVelocity + k3v * dt;

        // Final RK4 update
        glm::dvec2 newVelocity = initialVelocity + (k1v + 2.0 * k2v + 2.0 * k3v + k4v) * dt / 6.0;
        glm::dvec2 newPosition = initialPosition + (k1x + 2.0 * k2x + 2.0 * k3x + k4x) * dt / 6.0;

        // Handle boundary collisions
        HandleBoundaryCollision(newPosition, newVelocity, boundingBox);

        // Update atom state
        atom.SetVelocity(newVelocity);
        atom.SetPosition(newPosition);
    }

    void Integrator::LeapFrogStep(Atom& atom, const size_t atomIndex, const double dt,
                                 const std::vector<Atom>& allAtoms,
                                 const BoundingBox& boundingBox,
                                 const ForceCalculator& forceCalc)
    {
        // Handle collisions first
        HandleCollisions(atom, atomIndex, allAtoms, forceCalc);

        // Calculate current acceleration
        const glm::dvec2 totalForce = forceCalc.CalculateTotalForce(atom, allAtoms, atomIndex);
        const glm::dvec2 acceleration = totalForce / atom.GetMassD();

        // Leap-frog integration
        // v(t+dt/2) = v(t-dt/2) + a(t)*dt
        // x(t+dt) = x(t) + v(t+dt/2)*dt

        // For the first step, we need to calculate v(t-dt/2) from v(t)
        // This is already handled in velocity initialization

        glm::dvec2 newVelocity = atom.GetVelocityD() + acceleration * dt;
        glm::dvec2 newPosition = atom.GetPositionD() + newVelocity * dt;

        // Handle boundary collisions
        HandleBoundaryCollision(newPosition, newVelocity, boundingBox);

        // Update atom state
        atom.SetVelocity(newVelocity);
        atom.SetPosition(newPosition);
    }

    void Integrator::VelocityVerletStep(Atom& atom, const size_t atomIndex, const double dt,
                                       const std::vector<Atom>& allAtoms,
                                       const BoundingBox& boundingBox,
                                       const ForceCalculator& forceCalc)
    {
        // Handle collisions first
        HandleCollisions(atom, atomIndex, allAtoms, forceCalc);

        // Calculate current acceleration
        const glm::dvec2 currentForce = forceCalc.CalculateTotalForce(atom, allAtoms, atomIndex);
        const glm::dvec2 currentAcceleration = currentForce / atom.GetMassD();

        // Store current state
        const glm::dvec2 currentPosition = atom.GetPositionD();
        const glm::dvec2 currentVelocity = atom.GetVelocityD();

        // Velocity-Verlet integration (first half)
        // x(t+dt) = x(t) + v(t)*dt + 0.5*a(t)*dt^2
        glm::dvec2 newPosition = currentPosition + currentVelocity * dt + 0.5 * currentAcceleration * dt * dt;

        // Handle boundary collisions for position
        glm::dvec2 tempVelocity = currentVelocity;
        HandleBoundaryCollision(newPosition, tempVelocity, boundingBox);

        // Update position temporarily to calculate a new force
        atom.SetPosition(newPosition);

        // Calculate new acceleration at the new position
        const glm::dvec2 newForce = forceCalc.CalculateTotalForce(atom, allAtoms, atomIndex);
        const glm::dvec2 newAcceleration = newForce / atom.GetMassD();

        // Velocity-Verlet integration (second half)
        // v(t+dt) = v(t) + 0.5*[a(t) + a(t+dt)]*dt
        glm::dvec2 newVelocity = currentVelocity + 0.5 * (currentAcceleration + newAcceleration) * dt;

        // Handle boundary collisions for velocity
        HandleBoundaryCollision(newPosition, newVelocity, boundingBox);

        // Update atom state
        atom.SetVelocity(newVelocity);
        atom.SetPosition(newPosition);
    }

    double Integrator::AdaptiveTimeStep(const Atom& atom, const size_t atomIndex, const double dt,
                                       const std::vector<Atom>& allAtoms,
                                       const ForceCalculator& forceCalc) const
    {
        // Calculate current acceleration
        const glm::dvec2 currentAcceleration = ComputeAcceleration(atom, atomIndex,
                                                                  atom.GetPositionD(),
                                                                  atom.GetVelocityD(),
                                                                  allAtoms, forceCalc);

        // Estimate error based on acceleration magnitude
        const double accelerationMagnitude = glm::length(currentAcceleration);

        if (accelerationMagnitude < 1e-15) {
            return m_maxTimeStep;
        }

        // Calculate adaptive time step based on error tolerance
        // dt_adaptive = sqrt(2 * tolerance / |a|)
        double adaptiveDt = std::sqrt(2.0 * m_errorTolerance / accelerationMagnitude);

        // Clamp to min/max bounds
        adaptiveDt = std::clamp(adaptiveDt, m_minTimeStep, m_maxTimeStep);

        // Don't increase a time step too rapidly
        adaptiveDt = std::min(adaptiveDt, dt * 1.5);

        return adaptiveDt;
    }

    glm::dvec2 Integrator::ComputeAcceleration(const Atom& atom, const size_t atomIndex,
                                              const glm::dvec2& position, const glm::dvec2& velocity,
                                              const std::vector<Atom>& allAtoms,
                                              const ForceCalculator& forceCalc)
    {
        // Create a temporary atom with the given position and velocity for force calculation
        Atom tempAtom = atom;
        tempAtom.SetPosition(position);
        tempAtom.SetVelocity(velocity);

        // Calculate forces at this state
        const glm::dvec2 totalForce = forceCalc.CalculateTotalForce(tempAtom, allAtoms, atomIndex);

        // Return acceleration (F = ma, so a = F/m)
        return totalForce / atom.GetMassD();
    }

    void Integrator::HandleCollisions(Atom& atom, const size_t atomIndex,
                                     const std::vector<Atom>& allAtoms,
                                     const ForceCalculator& forceCalc)
    {
        // Handle collisions with other atoms
        for (size_t j = 0; j < allAtoms.size(); ++j) {
            if (atomIndex != j) {
                // Note: We need to cast away const to handle collisions
                // This is a design limitation that could be improved
                auto& otherAtom = const_cast<Atom&>(allAtoms[j]);
                forceCalc.HandleCollision(atom, otherAtom);
            }
        }
    }

    void Integrator::HandleBoundaryCollision(glm::dvec2& position, glm::dvec2& velocity,
                                            const BoundingBox& boundingBox, const double restitution)
    {
        // Check if the position is outside the bounding box
        if (!boundingBox.IsInside(position)) {
            // Handle X boundary collisions
            if (position.x < boundingBox.GetMinPoint().x) {
                position.x = boundingBox.GetMinPoint().x;
                velocity.x = -velocity.x * restitution;
            } else if (position.x > boundingBox.GetMaxPoint().x) {
                position.x = boundingBox.GetMaxPoint().x;
                velocity.x = -velocity.x * restitution;
            }

            // Handle Y boundary collisions
            if (position.y < boundingBox.GetMinPoint().y) {
                position.y = boundingBox.GetMinPoint().y;
                velocity.y = -velocity.y * restitution;
            } else if (position.y > boundingBox.GetMaxPoint().y) {
                position.y = boundingBox.GetMaxPoint().y;
                velocity.y = -velocity.y * restitution;
            }
        }
    }
}
