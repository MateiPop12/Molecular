#pragma once

#include <glm.hpp>

namespace Molecular{
    class PhysicsObject{
    public:
        PhysicsObject(glm::vec2 pos, float mass, float radius, float sigma, float epsilon, glm::vec4 color);

        void ApplyForce(glm::vec2 force);
        void Update(float ts,float boxSize);
        glm::vec2 GetPosition() const { return m_position; }

        float GetRadius() const { return m_radius; }
        float GetSigma() const { return m_sigma; }
        float GetEpsilon() const { return m_epsilon; }
        float GetMass() const { return m_mass; }
        glm::vec4 GetColor() const { return m_color; }

    private:
        glm::vec2 m_position{};
        glm::vec2 m_velocity{};
        glm::vec2 m_acceleration{};

        glm::vec4 m_color;

        float m_mass{};
        float m_radius;  // Atomic radius
        float m_sigma;   // Equilibrium distance
        float m_epsilon; // Strength of attraction
    };
}
