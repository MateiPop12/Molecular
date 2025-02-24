#include "PhysicsObject.h"


Molecular::PhysicsObject::PhysicsObject(glm::vec2 position, float mass)
        : m_position(position), m_velocity(0.0f), m_acceleration(0.0f), m_mass(mass)
{
}

void Molecular::PhysicsObject::ApplyForce(glm::vec2 force) {
    m_acceleration += force / m_mass;
}

void Molecular::PhysicsObject::Update(float ts) {
    m_velocity += m_acceleration * ts;
    m_position += m_velocity * ts;
    m_acceleration = glm::vec2(0.0f);

    if (m_position.y < -1.0f) {
        m_position.y = -1.0f;
        m_velocity.y = 0.0f;
    }
}
