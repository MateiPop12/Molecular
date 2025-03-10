#include "PhysicsWorld.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/norm.hpp>
#include <Core/Log.h>

namespace Molecular{

    const std::unordered_map<std::string, AtomProperties> AtomDatabase = {
            {"H", {1.008f, 2.5f, 0.65f, 0.53f, glm::vec4(0.5f, 0.0f, 0.5f, 1.0f)}},  // Hydrogen (Purple)
            {"O", {16.00f, 3.0f, 0.85f, 0.60f, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)}},  // Oxygen (Green)
            {"C", {12.01f, 3.4f, 0.35f, 0.70f, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)}},  // Carbon (Black)
            {"N", {14.01f, 3.3f, 0.75f, 0.65f, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)}}   // Nitrogen (Blue)
    };


    void PhysicsWorld::AddObject(const glm::vec2& position, float mass, float radius, float sigma, float epsilon, glm::vec4 color) {
        m_objects.emplace_back(position, mass, radius, sigma, epsilon, color);
    }

    void PhysicsWorld::AddObject(const glm::vec2& position, const std::string& element) {
        auto it = AtomDatabase.find(element);
        if (it != AtomDatabase.end()) {
            const AtomProperties& props = it->second;
            m_objects.emplace_back(position, props.mass, props.radius, props.sigma, props.epsilon, props.color);
        } else {
            std::cerr << "Error: Unknown element '" << element << "'\n";
        }
    }

    void PhysicsWorld::Update(float ts, float boxSize) {
        ComputeLennardJonesForces();
        SolveBonds();
        for (auto& obj : m_objects)
            obj.Update(ts,boxSize);
    }

    void PhysicsWorld::ComputeLennardJonesForces() {
        const float cutoffDistance = 3.0f;
        const float minDistance = 0.8f;

        //TODO: Optimize for loop
        for (size_t i = 0; i < m_objects.size(); ++i) {
            for (size_t j = i + 1; j < m_objects.size(); ++j) {
                PhysicsObject& objA = m_objects[i];
                PhysicsObject& objB = m_objects[j];

                glm::vec2 direction = objB.GetPosition() - objA.GetPosition();
                float distanceSquared = glm::length2(direction);

                float sigma = (objA.GetSigma() + objB.GetSigma()) / 2.0f;   // Geometric mean
                float epsilon = sqrt(objA.GetEpsilon() * objB.GetEpsilon()); // Arithmetic mean

                if (distanceSquared > cutoffDistance * cutoffDistance ||
                    distanceSquared < 0.0001f) continue;

                float distance = sqrt(distanceSquared);

                if (distance < minDistance) {
                    distance = minDistance;
                }

                glm::vec2 forceDirection = direction / distance; // Normalize

                float forceMagnitude = 24.0f * epsilon * (
                        2.0f * pow(sigma / distance, 13) - pow(sigma / distance, 7)
                ) / distance;

                glm::vec2 force = forceMagnitude * forceDirection;

                objA.ApplyForce(force);
                objB.ApplyForce(-force);
            }
        }
    }

    void PhysicsWorld::AddBond(size_t indexA, size_t indexB, float length, float stiffness) {
        m_bonds.push_back({indexA, indexB, length, stiffness});
    }

    void PhysicsWorld::SolveBonds() {
        for (auto& bond : m_bonds) {
            PhysicsObject& objA = m_objects[bond.atomA];
            PhysicsObject& objB = m_objects[bond.atomB];

            glm::vec2 direction = objB.GetPosition() - objA.GetPosition();
            float currentDistance = glm::length(direction);
            if (currentDistance < 0.0001f) continue; // Prevent division by zero

            float displacement = currentDistance - bond.length;
            glm::vec2 correction = (displacement / currentDistance) * direction;

            // Apply correction force proportionally
            objA.SetPosition(objA.GetPosition() + correction * 0.5f * bond.stiffness);
            objB.SetPosition(objB.GetPosition() - correction * 0.5f * bond.stiffness);
        }
    }
}