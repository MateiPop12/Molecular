#include "Atom.h"

namespace Molecular{

    void Atom::AddBond(Atom *other) {
        if (!IsBondedTo(other)) {
            m_bondedAtoms.push_back(other);
        }
    }

    bool Atom::IsBondedTo(Atom *other) {
        return std::find(m_bondedAtoms.begin(), m_bondedAtoms.end(), other) != m_bondedAtoms.end();
    }
}