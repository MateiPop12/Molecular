#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/norm.hpp>
#include "Atom.h"


//Molecular::Atom::Atom(glm::vec2 pos, float mass, float radius, float sigma, float epsilon, glm::vec4 color)
//        : m_position(pos), m_velocity(0.0f), m_acceleration(0.0f),
//          m_mass(mass), m_radius(radius), m_sigma(sigma), m_epsilon(epsilon),
//          m_color(color)
//{
//}
//
//void Molecular::Atom::Update(float ts, float boxSize) {
//    m_velocity += m_acceleration * ts;
//
//    // Cap kinetic energy
//    float maxEnergy = 1.2f; // Set an energy threshold
//    float kineticEnergy = 0.5f * m_mass * glm::length2(m_velocity);
//
//    if (kineticEnergy > maxEnergy) {
//        float scaleFactor = sqrt(maxEnergy / kineticEnergy);
//        m_velocity *= scaleFactor; // Reduce velocity to fit within max energy
//    }
//
//
//    m_position += m_velocity * ts;
//    m_acceleration = glm::vec2(0.0f); // Reset acceleration (Newton’s First Law)
//
//    // Check X boundaries
//    if (m_position.x - m_radius < -boxSize || m_position.x + m_radius > boxSize) {
//        m_velocity.x *= -1.0f; // Reverse velocity (elastic collision)
//        m_position.x = glm::clamp(m_position.x, -boxSize + m_radius, boxSize - m_radius);
//    }
//
//    // Check Y boundaries
//    if (m_position.y - m_radius < -boxSize || m_position.y + m_radius > boxSize) {
//        m_velocity.y *= -1.0f; // Reverse velocity (elastic collision)
//        m_position.y = glm::clamp(m_position.y, -boxSize + m_radius, boxSize - m_radius);
//    }
//}
