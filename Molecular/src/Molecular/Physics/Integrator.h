#pragma once

#include "Atom.h"
#include "BoundingBox.h"
#include "ForceCalculator.h"

namespace Molecular
{
    enum class IntegrationMethod {
        Euler,
        RungeKutta4
    };

    class Integrator
    {
        public:

        explicit Integrator(IntegrationMethod method = IntegrationMethod::RungeKutta4);

        void Integrate(Atom& atom, size_t atomIndex, double dt,const std::vector<Atom>& allAtoms,const BoundingBox& boundingBox,ForceCalculator& forceCalc) const;

        static void HandleBoundaryCollision(glm::dvec2& position, glm::dvec2& velocity, const BoundingBox& boundingBox);

        void SetIntegrationMethod(IntegrationMethod method) { m_method = method; }
        [[nodiscard]] IntegrationMethod GetIntegrationMethod() const { return m_method; }

        private:
        static void EulerStep(Atom& atom, size_t atomIndex, double dt,const std::vector<Atom>& allAtoms, const BoundingBox& boundingBox, const ForceCalculator& forceCalc);
        static void RungeKutta4Step(Atom& atom, size_t atomIndex, double dt, const std::vector<Atom>& allAtoms, const BoundingBox& boundingBox,ForceCalculator& forceCalc);

        static glm::dvec2 ComputeAcceleration(const Atom& atom, size_t atomIndex,const glm::dvec2& position, const glm::dvec2& velocity, const std::vector<Atom>& allAtoms, const ForceCalculator& forceCalc);

        IntegrationMethod m_method;
    };
}
