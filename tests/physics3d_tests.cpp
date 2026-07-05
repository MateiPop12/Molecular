#include "vendor/doctest/doctest.h"

#include "Molecular/Physics3D/SubatomicParticle.h"
#include "Molecular/Physics3D/ForceCalculator3D.h"
#include "Molecular/Physics3D/Integrator.h"

#include <cmath>
#include <vector>

using namespace Molecular;

namespace
{
    SubatomicParticle MakeAt(ParticleType type, const glm::dvec3& pos_fm)
    {
        SubatomicParticle p(type);
        p.SetPosition(pos_fm);
        p.SetVelocity({0.0, 0.0, 0.0});
        return p;
    }
}

// ---------------------------------------------------------------------------
// Coulomb force — directions, magnitude, neutrality
// ---------------------------------------------------------------------------

TEST_CASE("Coulomb: like charges repel along the separation axis")
{
    const ForceCalculator3D fc;
    const auto a = MakeAt(ParticleType::PROTON, {2.0, 0.0, 0.0});
    const auto b = MakeAt(ParticleType::PROTON, {0.0, 0.0, 0.0});

    const glm::dvec3 f_on_a = fc.CalculateCoulombForce(a, b);
    CHECK(f_on_a.x > 0.0);              // pushed away from b (+x)
    CHECK(std::abs(f_on_a.y) < 1e-30);
    CHECK(std::abs(f_on_a.z) < 1e-30);
}

TEST_CASE("Coulomb: opposite charges attract")
{
    const ForceCalculator3D fc;
    const auto e = MakeAt(ParticleType::ELECTRON, {10.0, 0.0, 0.0});
    const auto p = MakeAt(ParticleType::PROTON,   { 0.0, 0.0, 0.0});

    const glm::dvec3 f_on_e = fc.CalculateCoulombForce(e, p);
    CHECK(f_on_e.x < 0.0);              // pulled toward the proton (-x)
}

TEST_CASE("Coulomb: two protons at 1 fm feel ~230 N")
{
    // k*e^2/r^2 at r = 1 fm is 230.7 N; softening (eps = 0.1 fm) brings it
    // to ~228 N. The bracket is deliberately loose: it pins the UNITS
    // (fm in, Newtons out), not the softening formula. A unit slip would
    // miss this bracket by many orders of magnitude.
    const ForceCalculator3D fc;
    const auto a = MakeAt(ParticleType::PROTON, {1.0, 0.0, 0.0});
    const auto b = MakeAt(ParticleType::PROTON, {0.0, 0.0, 0.0});

    const double mag = glm::length(fc.CalculateCoulombForce(a, b));
    CHECK(mag > 200.0);
    CHECK(mag < 260.0);
}

TEST_CASE("Coulomb: neutrons feel and exert nothing")
{
    const ForceCalculator3D fc;
    const auto n = MakeAt(ParticleType::NEUTRON, {1.0, 0.0, 0.0});
    const auto p = MakeAt(ParticleType::PROTON,  {0.0, 0.0, 0.0});

    CHECK(glm::length(fc.CalculateCoulombForce(n, p)) == doctest::Approx(0.0));
    CHECK(glm::length(fc.CalculateCoulombForce(p, n)) == doctest::Approx(0.0));
}

// ---------------------------------------------------------------------------
// Strong force — range behaviour is the whole point of Yukawa
// ---------------------------------------------------------------------------

TEST_CASE("Strong force beats Coulomb repulsion at 1.5 fm (why nuclei bind)")
{
    const ForceCalculator3D fc;
    const auto a = MakeAt(ParticleType::PROTON, {1.5, 0.0, 0.0});
    const auto b = MakeAt(ParticleType::PROTON, {0.0, 0.0, 0.0});

    const double strong  = glm::length(fc.CalculateStrongForce(a, b));
    const double coulomb = glm::length(fc.CalculateCoulombForce(a, b));
    CHECK(strong > coulomb);
}

TEST_CASE("Strong force is negligible at 10 fm (short range)")
{
    const ForceCalculator3D fc;
    const auto a = MakeAt(ParticleType::PROTON, {10.0, 0.0, 0.0});
    const auto b = MakeAt(ParticleType::PROTON, { 0.0, 0.0, 0.0});

    const double strong  = glm::length(fc.CalculateStrongForce(a, b));
    const double coulomb = glm::length(fc.CalculateCoulombForce(a, b));
    CHECK(strong < 0.1 * coulomb);
}

TEST_CASE("Strong force skips electrons entirely")
{
    const ForceCalculator3D fc;
    const auto e = MakeAt(ParticleType::ELECTRON, {1.0, 0.0, 0.0});
    const auto p = MakeAt(ParticleType::PROTON,   {0.0, 0.0, 0.0});

    CHECK(glm::length(fc.CalculateStrongForce(e, p)) == doctest::Approx(0.0));
    CHECK(glm::length(fc.CalculateStrongForce(p, e)) == doctest::Approx(0.0));
}

TEST_CASE("Newton's third law: total force is antisymmetric")
{
    const ForceCalculator3D fc;
    const auto a = MakeAt(ParticleType::PROTON, {1.8, 0.3, -0.5});
    const auto b = MakeAt(ParticleType::PROTON, {0.0, 0.0,  0.0});

    const glm::dvec3 f_ab = fc.CalculateTotalForce(a, b);
    const glm::dvec3 f_ba = fc.CalculateTotalForce(b, a);
    const double residual = glm::length(f_ab + f_ba);
    CHECK(residual < 1e-9 * glm::length(f_ab));
}

// ---------------------------------------------------------------------------
// Integrator — invariants and unit consistency
// ---------------------------------------------------------------------------

TEST_CASE("Verlet: internal forces conserve total momentum over a step")
{
    Integrator integrator;
    const ForceCalculator3D fc;

    auto p = MakeAt(ParticleType::PROTON,   {  0.0, 0.0, 0.0});
    auto e = MakeAt(ParticleType::ELECTRON, {100.0, 0.0, 0.0});
    std::vector<PhysicsObject*> objects{&p, &e};

    integrator.VelocityVerlet(objects, 1e-23, fc);

    const glm::dvec3 totalP = p.GetMass() * p.GetVelocityD()
                            + e.GetMass() * e.GetVelocityD();
    const double scale = glm::length(p.GetMass() * p.GetVelocityD())
                       + glm::length(e.GetMass() * e.GetVelocityD());
    REQUIRE(scale > 0.0); // something must have moved at all
    CHECK(glm::length(totalP) < 1e-6 * scale);
}

TEST_CASE("Verlet: unit consistency — electron falls the right DISTANCE")
{
    // Electron at rest, 100 fm from a proton. Coulomb pull is 2.31e-2 N,
    // so a = F/m = 2.53e28 m/s^2, and in one dt = 1e-23 s step the electron
    // must move  0.5*a*dt^2 = 1.27e-18 m = 1.27e-3 fm  toward the proton.
    //
    // This test pins the UNIT CONVENTION across the core: positions are in
    // femtometres, accelerations come out of F/m in m/s^2 — someone has to
    // convert. If the integrator adds metre-displacements straight onto
    // fm positions (off by 1e15), the electron barely moves and this fails.
    Integrator integrator;
    const ForceCalculator3D fc;

    auto p = MakeAt(ParticleType::PROTON,   {  0.0, 0.0, 0.0});
    auto e = MakeAt(ParticleType::ELECTRON, {100.0, 0.0, 0.0});
    std::vector<PhysicsObject*> objects{&p, &e};

    integrator.VelocityVerlet(objects, 1e-23, fc);

    const double moved_fm = 100.0 - e.GetPositionD().x; // toward the proton
    CHECK(moved_fm > 1.0e-3);
    CHECK(moved_fm < 1.6e-3);
}