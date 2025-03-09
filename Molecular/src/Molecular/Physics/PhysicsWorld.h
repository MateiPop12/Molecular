#pragma once

#include "PhysicsObject.h"

namespace Molecular{
    class PhysicsWorld {
    public:
        void AddObject(const glm::vec2& position, float mass, float radius, float sigma, float epsilon, glm::vec4 color);
        void AddObject(const glm::vec2& position, const std::string& element);
        void Update(float ts, float boxSize);

        const std::vector<PhysicsObject>& GetObjects() const { return m_objects; }

    private:
        std::vector<PhysicsObject> m_objects;

        void ComputeLennardJonesForces();
    };
}
