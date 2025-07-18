#include "Integrator.h"

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
        switch (m_method) {
            case IntegrationMethod::Euler:
                EulerStep(atom, atomIndex, dt, allAtoms, boundingBox, forceCalc);
                break;
            case IntegrationMethod::RungeKutta4:
                RungeKutta4Step(atom, atomIndex, dt, allAtoms, boundingBox, forceCalc);
                break;
            default:
                // Default to Euler if the method is not recognized
                EulerStep(atom, atomIndex, dt, allAtoms, boundingBox, forceCalc);
                break;
        }
    }

    void Integrator::EulerStep(Atom& atom, const size_t atomIndex, const double dt,
                              const std::vector<Atom>& allAtoms,
                              const BoundingBox& boundingBox,
                              const ForceCalculator& forceCalc) {
        // Calculate total force on the atom
        const glm::dvec2 totalForce = forceCalc.CalculateTotalForce(atom, allAtoms, atomIndex);

        // Handle collisions with other atoms
        for (size_t j = 0; j < allAtoms.size(); ++j) {
            if (atomIndex != j) {
                // Note: We need to cast away const to handle collisions
                // This is a design limitation that could be improved
                auto& otherAtom = const_cast<Atom&>(allAtoms[j]);
                forceCalc.HandleCollision(atom, otherAtom);
            }
        }

        // Euler integration: v(t+dt) = v(t) + a(t)*dt, x(t+dt) = x(t) + v(t+dt)*dt
        const glm::dvec2 acceleration = totalForce / atom.GetMassD();
        glm::dvec2 newVelocity = atom.GetVelocityD() + acceleration * dt;
        glm::dvec2 newPosition = atom.GetPositionD() + newVelocity * dt;

        // Handle boundary collisions
        HandleBoundaryCollision(newPosition, newVelocity, boundingBox);

        // Update atom state
        atom.SetVelocity(newVelocity);
        atom.SetPosition(newPosition);
    }

    void Integrator::RungeKutta4Step(Atom& atom, size_t atomIndex, double dt,
                                    const std::vector<Atom>& allAtoms,
                                    const BoundingBox& boundingBox,
                                    ForceCalculator& forceCalc) {
        // Store initial state
        glm::dvec2 initialPosition = atom.GetPositionD();
        glm::dvec2 initialVelocity = atom.GetVelocityD();

        // Handle collisions first (similar to Euler)
        for (size_t j = 0; j < allAtoms.size(); ++j) {
            if (atomIndex != j) {
                auto& otherAtom = const_cast<Atom&>(allAtoms[j]);
                forceCalc.HandleCollision(atom, otherAtom);
            }
        }

        // RK4 integration steps
        // k1: derivatives at t
        glm::dvec2 k1v = ComputeAcceleration(atom, atomIndex, initialPosition, initialVelocity, allAtoms, forceCalc) * dt;
        glm::dvec2 k1x = initialVelocity * dt;

        // k2: derivatives at t + dt/2
        glm::dvec2 k2v = ComputeAcceleration(atom, atomIndex, initialPosition + k1x * 0.5, initialVelocity + k1v * 0.5, allAtoms, forceCalc) * dt;
        glm::dvec2 k2x = (initialVelocity + k1v * 0.5) * dt;

        // k3: derivatives at t + dt/2 (using k2)
        glm::dvec2 k3v = ComputeAcceleration(atom, atomIndex, initialPosition + k2x * 0.5, initialVelocity + k2v * 0.5, allAtoms, forceCalc) * dt;
        glm::dvec2 k3x = (initialVelocity + k2v * 0.5) * dt;

        // k4: derivatives at t + dt
        glm::dvec2 k4v = ComputeAcceleration(atom, atomIndex, initialPosition + k3x, initialVelocity + k3v, allAtoms, forceCalc) * dt;
        glm::dvec2 k4x = (initialVelocity + k3v) * dt;

        // Final RK4 update
        glm::dvec2 newVelocity = initialVelocity + (k1v + 2.0 * k2v + 2.0 * k3v + k4v) / 6.0;
        glm::dvec2 newPosition = initialPosition + (k1x + 2.0 * k2x + 2.0 * k3x + k4x) / 6.0;

        // Handle boundary collisions
        HandleBoundaryCollision(newPosition, newVelocity, boundingBox);

        // Update atom state
        atom.SetVelocity(newVelocity);
        atom.SetPosition(newPosition);
    }

    glm::dvec2 Integrator::ComputeAcceleration(const Atom& atom, const size_t atomIndex,
                                              const glm::dvec2& position, const glm::dvec2& velocity,
                                              const std::vector<Atom>& allAtoms,
                                              const ForceCalculator& forceCalc) {
        // Create a temporary atom with the given position and velocity for force calculation
        Atom tempAtom = atom;
        tempAtom.SetPosition(position);
        tempAtom.SetVelocity(velocity);

        // Calculate forces at this state
        const glm::dvec2 totalForce = forceCalc.CalculateTotalForce(tempAtom, allAtoms, atomIndex);

        // Return acceleration (F = ma, so a = F/m)
        return totalForce / atom.GetMassD();
    }

    void Integrator::HandleBoundaryCollision(glm::dvec2& position, glm::dvec2& velocity,
                                            const BoundingBox& boundingBox) {
        // Check if the position is outside the bounding box
        if (!boundingBox.IsInside(position)) {
            // Handle X boundary collisions
            if (position.x < boundingBox.GetMinPoint().x || position.x > boundingBox.GetMaxPoint().x) {
                velocity.x *= -1.0;
            }

            // Handle Y boundary collisions
            if (position.y < boundingBox.GetMinPoint().y || position.y > boundingBox.GetMaxPoint().y) {
                velocity.y *= -1.0;
            }

            // Clamp position to stay within bounds
            position = glm::clamp(position, boundingBox.GetMinPoint(), boundingBox.GetMaxPoint());
        }
    }
}