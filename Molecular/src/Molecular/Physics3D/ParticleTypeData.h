#pragma once
#include "molpch.h"

namespace Molecular
{
    enum class ParticleType : uint8_t { PROTON, NEUTRON, ELECTRON};

    struct ParticleTypeData
    {
        double mass_kg;         // kilograms, for physics calculations
        double mass_mev;        // MeV/c^2, reference
        float charge_C;         // Coulombs
        uint8_t spin_x2;        // store spin * 2 (0.5 -> 1, 1 -> 2)
        float magnetic_moment;  // in nuclear magnetons or Bohr magnetons
        double lifetime_s;      // seconds, INFINITY if stable
    };

    constexpr std::array<ParticleTypeData, 3> PARTICLE_TYPES = {{
        {1.67262192369e-27, 938.27208816, 1.602176634e-19f,1, 2.79284734462f, std::numeric_limits<double>::infinity()}, // PROTON
        {1.67492749804e-27, 939.56542052,             0.0f,1,   -1.91304273f,                                   880.2}, // NEUTRON
        { 9.1093837015e-31,  0.510998950,-1.602176634e-19f,1,-1.00115965218f, std::numeric_limits<double>::infinity()}  // ELECTRON
    }};
}
//CODATA 2018/2022 values