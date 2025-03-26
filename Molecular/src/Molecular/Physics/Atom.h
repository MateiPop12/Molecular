#pragma once

#include <glm.hpp>

namespace Molecular{
    class Atom{
    public:
        Atom(glm::vec2 position, glm::vec2 velocity, float mass,
             float vanDerWaalsRadius, float bondLength, float epsilon, float sigma)
                : m_position(position), m_velocity(velocity), m_mass(mass),
                  m_vanDerWaalsRadius(vanDerWaalsRadius), m_CovalentBondLength(bondLength),
                  m_epsilon(epsilon), m_sigma(sigma) {}

        glm::vec2 GetPosition() const { return m_position; }
        glm::vec2 GetVelocity() const { return m_velocity; }
        float GetVanDerWaalsRadius() const { return m_vanDerWaalsRadius; }
        float GetCovalentBondLength() const { return m_CovalentBondLength; }
        float GetEpsilon() const { return m_epsilon; }
        float GetSigma() const { return m_sigma; }
        float GetMass() const { return m_mass; }

        void SetPosition(const glm::vec2& position) { m_position = position; }
        void SetVelocity(const glm::vec2& velocity) { m_velocity = velocity; }

    private:
        glm::vec2 m_position{};
        glm::vec2 m_velocity{};

        float m_vanDerWaalsRadius{};    // Non-bonded interaction distance
        float m_CovalentBondLength{};           // Bonding interaction distance
        float m_epsilon{};              // Interaction strength
        float m_sigma{};                // Distance where potential is zero
        float m_mass{};                 // Atomic mass
    };
}
