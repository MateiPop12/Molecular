#include "ForceCalculator3D.h"

namespace Molecular
{
    // -----------------------------------------------------------------------
    // Physical constants (SI)
    // -----------------------------------------------------------------------
    static constexpr double k_e       = 8.9875517923e9;    // Coulomb constant  (N·m²/C²)
    static constexpr double FM_TO_M   = 1.0e-15;           // 1 fm in metres

    // Yukawa / residual strong-force parameters
    // V(r) = -g² * exp(-r/r0) / r   (attractive, short-range)
    // F(r) = -dV/dr acted on 'a' along r_hat toward 'b'
    //
    // g²  ~ 14.5 MeV·fm  (pi-meson coupling, standard nuclear physics value)
    // r0  ~ 1.4 fm        (pion Compton wavelength ≈ ħc / m_π·c²)
    //
    // Converting g² to SI:   1 MeV·fm = 1e6 * 1.602176634e-19 J * 1e-15 m
    static constexpr double YUKAWA_G2_SI = 14.5 * 1.602176634e-19 * 1e6 * FM_TO_M; // J·m
    static constexpr double YUKAWA_R0_M  = 1.4 * FM_TO_M;                          // metres

    // -----------------------------------------------------------------------

    ForceCalculator3D::ForceCalculator3D(double epsilon_fm)
        : m_epsilon_m(epsilon_fm * FM_TO_M)
    {
    }

    // -----------------------------------------------------------------------
    // Coulomb force on 'a' due to 'b'
    // Positions in femtometres; returned force in Newtons.
    // -----------------------------------------------------------------------
    glm::dvec3 ForceCalculator3D::CalculateCoulombForce(const PhysicsObject& a,
                                                      const PhysicsObject& b) const
    {
        // Convert positions from fm to metres
        const glm::dvec3 pos_a = a.GetPositionD() * FM_TO_M;
        const glm::dvec3 pos_b = b.GetPositionD() * FM_TO_M;

        const glm::dvec3 r_vec = pos_a - pos_b;               // vector from b to a (m)

        // Softened distance² = |r|² + ε²  →  avoids 1/0 at coincident positions
        const double r2_soft = glm::dot(r_vec, r_vec) + m_epsilon_m * m_epsilon_m;
        const double r_soft  = std::sqrt(r2_soft);

        // F = k_e * q_a * q_b / r²  (signed; repulsive when charges are equal)
        // Direction: along r_vec (away from b toward a)
        const double magnitude = k_e * a.GetCharge() * b.GetCharge() / r2_soft;

        return (magnitude / r_soft) * r_vec; // = magnitude * r_hat
    }

    // -----------------------------------------------------------------------
    // Yukawa residual strong force on 'a' due to 'b'
    // Positions in femtometres; returned force in Newtons.
    // Only nucleons (proton/neutron) interact via the strong force;
    // electrons are skipped by returning zero.
    // -----------------------------------------------------------------------
    glm::dvec3 ForceCalculator3D::CalculateStrongForce(const PhysicsObject& a,
                                                     const PhysicsObject& b) const
    {
        // Electrons don't participate in the residual nuclear strong force
        if (std::abs(a.GetMass() - 9.1093837015e-31) < 1e-33 ||
            std::abs(b.GetMass() - 9.1093837015e-31) < 1e-33)
        {
            return glm::dvec3(0.0);
        }

        const glm::dvec3 pos_a = a.GetPositionD() * FM_TO_M;
        const glm::dvec3 pos_b = b.GetPositionD() * FM_TO_M;

        const glm::dvec3 r_vec = pos_a - pos_b;

        const double r2_soft = glm::dot(r_vec, r_vec) + m_epsilon_m * m_epsilon_m;
        const double r_soft  = std::sqrt(r2_soft);

        // Yukawa potential derivative:
        // F(r) = +g² * exp(-r/r0) * (1/r + 1/r0) / r   (attractive → toward b)
        // The '+' becomes attractive when we negate the direction (toward b = -r_hat)
        const double exp_term = std::exp(-r_soft / YUKAWA_R0_M);
        const double f_mag    = YUKAWA_G2_SI * exp_term * (1.0 / r_soft + 1.0 / YUKAWA_R0_M) / r_soft;

        return -f_mag * (r_vec / r_soft); // attractive: toward b
    }

    // -----------------------------------------------------------------------
    // Total force on 'a' due to 'b'
    // -----------------------------------------------------------------------
    glm::dvec3 ForceCalculator3D::CalculateTotalForce(const PhysicsObject& a,
                                                    const PhysicsObject& b) const
    {
        return CalculateCoulombForce(a, b) + CalculateStrongForce(a, b);
    }
}
