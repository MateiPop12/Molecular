#pragma once

#include "PhysicsObject.h"

namespace Molecular{

    struct AtomProperties{
        float mass;// atomic mass units
        float radius;// angstroms
        float sigma;// angstroms
        float epsilon;// kj/mol
        glm::vec4 color; // Atom color (RGBA)
    };

    struct Bond {
        size_t atomA;
        size_t atomB;
        float length; // Covalent bond length
        float stiffness; // Stiffness of the bond
    };

    class PhysicsWorld {
    public:
        void AddObject(const glm::vec2& position, float mass, float radius, float sigma, float epsilon, glm::vec4 color);
        void AddObject(const glm::vec2& position, const std::string& element);
        void AddBond(size_t indexA, size_t indexB, float length, float stiffness);
        const std::vector<Bond>& GetBonds() const { return m_bonds; }
        void SolveBonds(); // New function to enforce bond constraints
        void Update(float ts, float boxSize);

        const std::vector<PhysicsObject>& GetObjects() const { return m_objects; }

    private:
        std::vector<PhysicsObject> m_objects;
        std::vector<Bond> m_bonds;

        void ComputeLennardJonesForces();
    };
}
