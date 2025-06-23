#include "Atom.h"

namespace Molecular{

    void Atom::AddBond(Atom* other) {
        if (CanBondWith(other)) {
            m_bondedAtoms.push_back(other);
            other->m_bondedAtoms.push_back(this);
        }
    }

    void Atom::TryFormBond(Atom* other) {
        if (CanFormBondWith(other)) {
            AddBond(other);
        }
    }

    void Atom::BreakBond(Atom* other) {
        // Remove from this atom's bond list
        m_bondedAtoms.erase(
                std::remove(m_bondedAtoms.begin(), m_bondedAtoms.end(), other),
                m_bondedAtoms.end()
        );

        // Remove from other atom's bond list
        other->m_bondedAtoms.erase(
                std::remove(other->m_bondedAtoms.begin(), other->m_bondedAtoms.end(), this),
                other->m_bondedAtoms.end()
        );
    }

    bool Atom::CanBondWith(const Atom* other) const {
        if (other == this) return false;
        if (!CanFormBond() || !other->CanFormBond()) return false;

        double deltaEN = std::abs(m_electronegativity - other->m_electronegativity);
        return deltaEN <= 1.7; // Simple covalent bond rule
    }

    bool Atom::CanFormBondWith(const Atom* other) const {
        if (!other || other == this) return false;
        if (IsBondedTo(other)) return false; // Already bonded
        if (!CanFormBond() || !other->CanFormBond()) return false;
        if (!IsWithinBondingRange(other)) return false;

        // Check electronegativity compatibility
        double deltaEN = std::abs(m_electronegativity - other->m_electronegativity);
        return deltaEN <= 1.7; // Covalent bond threshold
    }

    bool Atom::IsBondedTo(const Molecular::Atom *other) const {
        for (const Atom* bonded : m_bondedAtoms) {
            if (bonded == other) return true;
        }
        return false;
    }

    bool Atom::IsWithinBondingRange(const Atom* other) const {
        if (!other || other == this) return false;

        double distance = glm::length(m_position - other->m_position);
        double bondingDistance = (m_CovalentBondLength + other->m_CovalentBondLength) * 1.5; // 20% tolerance

        return distance <= bondingDistance;
    }

    bool Atom::ShouldBreakBondWith(const Atom* other) const {
        if (!IsBondedTo(other)) return false;

        double distance = glm::length(m_position - other->m_position);
        double maxBondDistance = (m_CovalentBondLength + other->m_CovalentBondLength) * 2.0; // Break if 2x normal distance

        return distance > maxBondDistance;
    }

}