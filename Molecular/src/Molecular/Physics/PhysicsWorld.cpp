#include "PhysicsWorld.h"

namespace Molecular{

    void PhysicsWorld::AddObject(const glm::vec2 &position, float mass) {
        m_objects.emplace_back(position, mass);
    }

    void PhysicsWorld::ApplyGlobalForce(glm::vec2 force) {
        for (auto& obj : m_objects)
            obj.ApplyForce(force);
    }

    void PhysicsWorld::Update(float ts) {
        for (auto& obj : m_objects)
            obj.Update(ts);
    }
}