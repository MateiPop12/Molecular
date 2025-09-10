#include "SubatomicParticle.h"

namespace Molecular
{
    SubatomicParticle::SubatomicParticle(const ParticleType type)
        : PhysicsObject(type), m_radius(GetDefaultRadius(type)), m_color(GetDefaultColor(type))
    {
    }

    glm::vec4 SubatomicParticle::GetDefaultColor(const ParticleType type)
    {
        switch (type)
        {
        case ParticleType::PROTON:
            return {1.0f, 0.2f, 0.2f,1.0f}; // Red
        case ParticleType::NEUTRON:
            return {0.7f, 0.7f, 0.7f,1.0f}; // Gray
        case ParticleType::ELECTRON:
            return {0.2f, 0.2f, 1.0f,1.0f}; // Blue
        default:
            return {1.0f, 1.0f, 0.0f,1.0f}; // fallback
        }
    }

    float SubatomicParticle::GetDefaultRadius(const ParticleType type)
    {
        switch (type)
        {
        case ParticleType::PROTON:
            return 0.8768f; // ~0.876 fm (proton charge radius)
        case ParticleType::NEUTRON:
            return 0.8751f; // ~0.875 fm (neutron charge radius)
        case ParticleType::ELECTRON:
            return 0.1f;
        default:
            return 1.0f; // 1 am
        }
    }
}
