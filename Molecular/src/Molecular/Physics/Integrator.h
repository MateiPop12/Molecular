#pragma once

#include "Atom.h"
#include "BoundingBox.h"
#include "ForceCalculator.h"

namespace Molecular
{
    enum class IntegrationMethod {
        Euler,
        RungeKutta4,
        LeapFrog,
        VelocityVerlet
    };

    class Integrator
    {
    public:
        explicit Integrator(IntegrationMethod method = IntegrationMethod::VelocityVerlet);

        void Integrate(Atom& atom, size_t atomIndex, double dt,
                              const std::vector<Atom>& allAtoms,
                              const BoundingBox& boundingBox,
                              ForceCalculator& forceCalc) const;

        static void InitializeVerlet(std::vector<Atom>& atoms, double dt,
                                     const BoundingBox& boundingBox,
                                     const ForceCalculator& forceCalc);

        static void HandleBoundaryCollision(glm::dvec2& position, glm::dvec2& velocity,
                                                  const BoundingBox& boundingBox, double restitution = 0.9);

        void SetIntegrationMethod(IntegrationMethod method) { m_method = method; }
        [[nodiscard]] IntegrationMethod GetIntegrationMethod() const { return m_method; }

        // Adaptive time stepping
        void SetAdaptiveTimeStep(bool adaptive) { m_useAdaptiveTimeStep = adaptive; }
        void SetMaxTimeStep(double maxDt) { m_maxTimeStep = maxDt; }
        void SetMinTimeStep(double minDt) { m_minTimeStep = minDt; }
        void SetErrorTolerance(double tolerance) { m_errorTolerance = tolerance; }

        [[nodiscard]] bool IsAdaptiveTimeStep() const { return m_useAdaptiveTimeStep; }
        [[nodiscard]] double GetMaxTimeStep() const { return m_maxTimeStep; }
        [[nodiscard]] double GetMinTimeStep() const { return m_minTimeStep; }
        [[nodiscard]] double GetErrorTolerance() const { return m_errorTolerance; }

    private:
        // Integration methods
        static void EulerStep(Atom& atom, size_t atomIndex, double dt,
                             const std::vector<Atom>& allAtoms,
                             const BoundingBox& boundingBox,
                             const ForceCalculator& forceCalc);

        static void RungeKutta4Step(Atom& atom, size_t atomIndex, double dt,
                                   const std::vector<Atom>& allAtoms,
                                   const BoundingBox& boundingBox,
                                   ForceCalculator& forceCalc);

        static void VerletStep(Atom& atom, size_t atomIndex, double dt,
                              const std::vector<Atom>& allAtoms,
                              const BoundingBox& boundingBox,
                              const ForceCalculator& forceCalc);

        static void LeapFrogStep(Atom& atom, size_t atomIndex, double dt,
                                const std::vector<Atom>& allAtoms,
                                const BoundingBox& boundingBox,
                                const ForceCalculator& forceCalc);

        static void VelocityVerletStep(Atom& atom, size_t atomIndex, double dt,
                                      const std::vector<Atom>& allAtoms,
                                      const BoundingBox& boundingBox,
                                      const ForceCalculator& forceCalc);

        // Adaptive time stepping
        [[nodiscard]] double AdaptiveTimeStep(const Atom& atom, size_t atomIndex, double dt,
                               const std::vector<Atom>& allAtoms,
                               const ForceCalculator& forceCalc) const;

        // Helper methods
        static glm::dvec2 ComputeAcceleration(const Atom& atom, size_t atomIndex,
                                             const glm::dvec2& position,
                                             const glm::dvec2& velocity,
                                             const std::vector<Atom>& allAtoms,
                                             const ForceCalculator& forceCalc);

        static void HandleCollisions(Atom& atom, size_t atomIndex,
                                    const std::vector<Atom>& allAtoms,
                                    const ForceCalculator& forceCalc);

        // Member variables
        IntegrationMethod m_method;
        bool m_useAdaptiveTimeStep = false;
        double m_maxTimeStep = 1e-12;
        double m_minTimeStep = 1e-16;
        double m_errorTolerance = 1e-10;
    };
}
