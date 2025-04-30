#include "Atom.h"

namespace Molecular{

    void Atom::AddBond(Atom* other) {
    if (CanBondWith(other)) {
        m_bondedAtoms.push_back(other);
        other->m_bondedAtoms.push_back(this);
        }
    }

    bool Atom::CanBondWith(const Atom* other) const {
        if (other == this) return false;
        if (!CanFormBond() || !other->CanFormBond()) return false;

        double deltaEN = std::abs(m_electronegativity - other->m_electronegativity);
        return deltaEN <= 1.7; // Simple covalent bond rule
    }

    bool Atom::IsBondedTo(const Molecular::Atom *other) const {
        for (const Atom* bonded : m_bondedAtoms) {
            if (bonded == other) return true;
        }
        return false;
    }
}