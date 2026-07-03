#include "vendor/doctest/doctest.h"

#include "Molecular/Physics/Atom.h"

TEST_CASE("sanity: the harness runs")
{
    CHECK(1 + 1 == 2);
}

TEST_CASE("sanity: engine headers are reachable")
{
    Molecular::Atom atom("H", glm::dvec2(0.0, 0.0));
    CHECK(atom.GetMassD() == doctest::Approx(1.008));
}