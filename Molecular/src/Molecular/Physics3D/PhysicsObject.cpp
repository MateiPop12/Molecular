#include "PhysicsObject.h"

Molecular::PhysicsObject::PhysicsObject(ParticleType type)
    :m_type(type),
      m_typeData(&PARTICLE_TYPES[static_cast<size_t>(type)]),
      m_position(0.0),
      m_velocity(0.0),
      m_mass(m_typeData->mass_kg)
{
}
