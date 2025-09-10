#pragma once
#include "PhysicsObject.h"

namespace Molecular
{
    class SubatomicParticle : public PhysicsObject
    {
    public:
        explicit SubatomicParticle(ParticleType type);

        [[nodiscard]] float GetRadius() const { return m_radius; }
        [[nodiscard]] glm::vec3 GetColor() const { return m_color; }

    private:
        float m_radius;
        glm::vec4 m_color;

        [[nodiscard]] static glm::vec4 GetDefaultColor(ParticleType type);
        [[nodiscard]] static float GetDefaultRadius(ParticleType type);
    };
}
