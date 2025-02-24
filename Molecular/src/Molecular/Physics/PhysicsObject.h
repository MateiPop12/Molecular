#pragma once

#include <glm.hpp>

namespace Molecular{
    class PhysicsObject{
    public:
        PhysicsObject(glm::vec2 position, float mass);

        void ApplyForce(glm::vec2 force);
        void Update(float ts);

        glm::vec2 GetPosition() const { return m_position; }

    private:
        glm::vec2 m_position;
        glm::vec2 m_velocity;
        glm::vec2 m_acceleration;
        float m_mass;
    };
}
