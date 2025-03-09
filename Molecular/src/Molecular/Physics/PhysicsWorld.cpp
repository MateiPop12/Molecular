#include "PhysicsWorld.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/norm.hpp>
#include <Core/Log.h>

namespace Molecular{

    struct AtomProperties{
        float mass;// atomic mass units
        float radius;// angstroms
        float sigma;// angstroms
        float epsilon;// kj/mol
        glm::vec4 color; // Atom color (RGBA)
    };

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
        for (auto& obj : m_objects)
            obj.Update(ts,boxSize);
    }

    void PhysicsWorld::ComputeLennardJonesForces() {
        const float cutoffDistance = 5.0f;
        //const float maxForceMagnitude = 3.0f;

        //TODO: Optimize for loop
        for(size_t i = 0; i < m_objects.size(); ++i){
            for(size_t j = i + 1; j < m_objects.size(); ++j){
                PhysicsObject& objA = m_objects[i];
                PhysicsObject& objB = m_objects[j];

                glm::vec2 direction = objB.GetPosition() - objA.GetPosition();
                float distanceSquared = glm::length2(direction);
                float sigma = (objA.GetSigma() + objB.GetSigma()) / 2.0f;
                float epsilon = sqrt(objA.GetEpsilon() * objB.GetEpsilon());

                if (distanceSquared > cutoffDistance * cutoffDistance ||
                distanceSquared < 0.0000000000001f) continue;// Ignore far or overlapping atoms

                float distance = sqrt(distanceSquared);
                if (distance < 0.8f * sigma) distance = 0.8f * sigma; // Prevent atoms from getting too close

                glm::vec2 forceDirection = direction / distance; // Normalize

                float forceMagnitude = 24.0f * epsilon * (
                        2.0f * pow(sigma / distance, 13) - pow(sigma / distance, 7)
                ) / distance;

//                if (glm::length(forceDirection) * forceMagnitude > maxForceMagnitude) {
//                    forceMagnitude = maxForceMagnitude / glm::length(forceDirection);  // Scale down the force
//                }

                glm::vec2 force = forceMagnitude * forceDirection;

                //std::cout << "Force between atoms: (" << force.x << ", " << force.y << ")\n";

                objA.ApplyForce(force);
                objB.ApplyForce(-force); // Newton's Third Law (equal & opposite reaction)
            }
        }
    }
}