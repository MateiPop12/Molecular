#include "Molecule.h"

namespace Molecular {

//    void Molecule::AddAtom(Atom* atom) {
//        if (!ContainsAtom(atom)) {
//            m_atoms.push_back(atom);
//        }
//    }
//
//    bool Molecule::ContainsAtom(const Atom* atom) const {
//        return std::find(m_atoms.begin(), m_atoms.end(), atom) != m_atoms.end();
//    }
//
//    bool Molecule::HasFreeBondSlots(Atom* atom) const {
//        auto it = maxBonds.find(atom->GetElement());
//        if (it == maxBonds.end()) return false;
//        return atom->GetBondCount() < it->second;
//    }
//
//    bool Molecule::CanAcceptAtom(const Atom& atom) const {
//        for (Atom* existing : m_atoms) {
//            if (HasFreeBondSlots(existing) &&
//                glm::length(existing->GetPositionD() - atom.GetPositionD()) <
//                (existing->GetVanDerWaalsRadiusD() + atom.GetVanDerWaalsRadiusD()) * 0.9) {
//                return true;
//            }
//        }
//        return false;
//    }
//
//    bool Molecule::TryBondAtoms(Atom* a, Atom* b) {
//        if (!a || !b || a == b || a->IsBondedTo(b)) return false;
//
//        if (HasFreeBondSlots(a) && HasFreeBondSlots(b)) {
//            a->AddBond(b);
//            b->AddBond(a);
//            AddAtom(a);
//            AddAtom(b);
//            return true;
//        }
//        return false;
//    }
}