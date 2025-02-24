#pragma once

#include "PhysicsObject.h"

namespace Molecular{
    class PhysicsWorld {
    public:
        void AddObject(const glm::vec2& position, float mass);
        void ApplyGlobalForce(glm::vec2 force);
        void Update(float ts);

        const std::vector<PhysicsObject>& GetObjects() const { return m_objects; }

    private:
        std::vector<PhysicsObject> m_objects;
    };
}
