#pragma once
#include "glm.hpp"
#include "ParticleTypeData.h"

namespace Molecular
{
    class PhysicsObject
    {
    public:
        explicit PhysicsObject(ParticleType type);

        [[nodiscard]] glm::dvec3 GetPositionD() const {return m_position;}
        [[nodiscard]] glm::dvec3 GetVelocityD() const {return m_velocity;}
        [[nodiscard]] double GetMass() const {return m_mass;}

        void SetPosition(const glm::dvec3& position) {m_position = position;}
        void SetVelocity(const glm::dvec3& velocity) {m_velocity = velocity;}

    private:
        ParticleType m_type;
        const ParticleTypeData* m_typeData;
        glm::dvec3 m_position;
        glm::dvec3 m_velocity;
        double m_mass;
    };
}
