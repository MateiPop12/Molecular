#include "Atom.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace Molecular{

    void Atom::AddBond(Atom* other) {
        if (CanBondWith(other)) {
            m_bondedAtoms.push_back(other);
            other->m_bondedAtoms.push_back(this);
            UpdateElectronPairGeometry();
            other->UpdateElectronPairGeometry();
        }
    }

    glm::dvec2 Atom::CalculateAngularForce(const Atom* centerAtom, const Atom* otherBonded) const
    {
        if (!centerAtom || !otherBonded) return glm::dvec2(0.0);

        // Vector from center to this atom
        glm::dvec2 r1 = m_position - centerAtom->m_position;
        // Vector from center to other bonded atom
        glm::dvec2 r2 = otherBonded->m_position - centerAtom->m_position;

        double r1_len = glm::length(r1);
        double r2_len = glm::length(r2);

        if (r1_len < 1e-10 || r2_len < 1e-10) return glm::dvec2(0.0);

        // Normalize vectors
        glm::dvec2 n1 = r1 / r1_len;
        glm::dvec2 n2 = r2 / r2_len;

        // Calculate current angle between bonds
        double cosTheta = glm::dot(n1, n2);
        cosTheta = glm::clamp(cosTheta, -1.0, 1.0);
        double currentAngle = std::acos(cosTheta);

        // Get ideal angle based on molecular geometry
        double idealAngle = centerAtom->GetIdealBondAngle();

        // Calculate angular deviation
        double angleDiff = currentAngle - idealAngle;

        // Spring constant for angular constraint
        const double k_angle = 100.0; // Adjust this value to control angular stiffness

        // Calculate force magnitude
        double forceMag = k_angle * angleDiff;

        // Calculate force direction (perpendicular to current bond direction)
        glm::dvec2 perpendicular(-n1.y, n1.x);

        // Determine correct direction based on angle deviation
        if (angleDiff > 0) {
            // Current angle is too large, need to rotate toward ideal
            if (glm::dot(perpendicular, n2) < 0) {
                perpendicular = -perpendicular;
            }
        } else {
            // Current angle is too small, need to rotate away from ideal
            if (glm::dot(perpendicular, n2) > 0) {
                perpendicular = -perpendicular;
            }
        }

        return perpendicular * forceMag;
    }

    glm::dvec2 Atom::CalculateLonePairForce(const ElectronPair& lonePair, const std::vector<ElectronPair>& allPairs) const
    {
        if (lonePair.type != ElectronPairType::LonePair) return glm::dvec2(0.0);

        glm::dvec2 totalForce(0.0);
        const double k_lonePair = 50.0; // Force constant for lone pair repulsion

        for (const auto& otherPair : allPairs) {
            if (&otherPair == &lonePair) continue; // Skip self

            // Calculate angle between lone pair and other electron pair
            double cosTheta = glm::dot(lonePair.direction, otherPair.direction);
            cosTheta = glm::clamp(cosTheta, -1.0, 1.0);
            double currentAngle = std::acos(cosTheta);

            // Get ideal angle based on electron pair geometry
            double idealAngle = GetIdealAngleBetweenPairs(lonePair, otherPair);
            double angleDiff = currentAngle - idealAngle;

            // Only apply force if angle difference is significant
            if (std::abs(angleDiff) < 1e-3) continue;

            // Calculate force magnitude
            double forceMag = k_lonePair * angleDiff * 0.5;

            // Calculate perpendicular direction for force
            glm::dvec2 perpendicular(-lonePair.direction.y, lonePair.direction.x);

            // Determine correct direction
            if (angleDiff > 0) {
                if (glm::dot(perpendicular, otherPair.direction) < 0) {
                    perpendicular = -perpendicular;
                }
            } else {
                if (glm::dot(perpendicular, otherPair.direction) > 0) {
                    perpendicular = -perpendicular;
                }
            }

            totalForce += perpendicular * forceMag;
        }

        return totalForce;
    }

    glm::dvec2 Atom::CalculateAllLonePairForces() const
    {
        glm::dvec2 totalForce(0.0);

        // Get all electron pairs for this atom
        const auto& allPairs = m_electronPairs;

        // Calculate forces for each lone pair
        for (const auto& pair : allPairs) {
            if (pair.type == ElectronPairType::LonePair) {
                totalForce += CalculateLonePairForce(pair, allPairs);
            }
        }

        return totalForce;
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

    double Atom::GetIdealBondAngle() const
    {
        switch (GetMolecularGeometry()) {
        case MolecularGeometry::Linear:
            return M_PI; // 180 degrees
        case MolecularGeometry::Trigonal:
            return 2.0 * M_PI / 3.0; // 120 degrees
        case MolecularGeometry::Tetrahedral:
            return std::acos(-1.0/3.0); // 109.5 degrees
        case MolecularGeometry::TrigonalBipyramidal:
            return M_PI / 2.0; // 90 degrees (simplified)
        case MolecularGeometry::Octahedral:
            return M_PI / 2.0; // 90 degrees
        default:
            return M_PI / 2.0;
        }
    }

    double Atom::GetIdealAngleBetweenPairs(const ElectronPair& pair1, const ElectronPair& pair2) const
    {
        int totalPairs = m_electronPairs.size();

        // Base angles for different electron pair geometries
        double baseAngle = 0.0;

        switch (totalPairs) {
        case 2:
            baseAngle = M_PI; // 180° - Linear
            break;
        case 3:
            baseAngle = 2.0 * M_PI / 3.0; // 120° - Trigonal planar
            break;
        case 4:
            baseAngle = std::acos(-1.0/3.0); // 109.5° - Tetrahedral
            break;
        case 5:
            baseAngle = M_PI / 2.0; // 90° - Trigonal bipyramidal (simplified)
            break;
        case 6:
            baseAngle = M_PI / 2.0; // 90° - Octahedral
            break;
        default:
            baseAngle = 2.0 * M_PI / totalPairs; // Evenly distributed
            break;
        }

        // Lone pairs require more space than bonding pairs
        // This affects the ideal angles in molecules like water (H2O) and ammonia (NH3)

        bool bothLonePairs = (pair1.type == ElectronPairType::LonePair && pair2.type == ElectronPairType::LonePair);
        bool oneLonePair = (pair1.type == ElectronPairType::LonePair) != (pair2.type == ElectronPairType::LonePair);

        // Lone pairs repel more strongly, so they prefer larger angles
        if (bothLonePairs) {
            return baseAngle * 1.1; // Lone pair - lone pair: larger angle
        } else if (oneLonePair) {
            return baseAngle * 1.05; // Lone pair - bonding pair: slightly larger angle
        } else {
            return baseAngle; // Bonding pair - bonding pair: normal angle
        }
    }

    int Atom::GetLonePairCount() const
    {
        // Calculate lone pairs based on valence electrons and bonding electrons
        // This is a simplified model - in reality, it depends on the specific element

        int bondingElectrons = m_bondedAtoms.size(); // Assuming single bonds
        int valenceElectrons = 0;

        // Get valence electrons based on element type
        if (m_element == "H") valenceElectrons = 1;
        else if (m_element == "C") valenceElectrons = 4;
        else if (m_element == "N") valenceElectrons = 5;
        else if (m_element == "O") valenceElectrons = 6;
        else valenceElectrons = m_valence; // fallback

        int lonePairElectrons = valenceElectrons - bondingElectrons;
        return std::max(0, lonePairElectrons / 2); // Each lone pair has 2 electrons
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

    std::vector<glm::dvec2> Atom::GetOptimalBondDirections() const
    {
        std::vector<glm::dvec2> directions;
        int bondCount = m_bondedAtoms.size();

        switch (GetMolecularGeometry()) {
        case MolecularGeometry::Linear:
            directions.push_back(glm::dvec2(1.0, 0.0));
            directions.push_back(glm::dvec2(-1.0, 0.0));
            break;

        case MolecularGeometry::Trigonal:
            for (int i = 0; i < 3; ++i) {
                double angle = i * 2.0 * M_PI / 3.0;
                directions.push_back(glm::dvec2(std::cos(angle), std::sin(angle)));
            }
            break;

        case MolecularGeometry::Tetrahedral:
            // 2D approximation of tetrahedral (would need 3D for true tetrahedral)
            directions.push_back(glm::dvec2(1.0, 0.0));
            directions.push_back(glm::dvec2(-0.5, std::sqrt(3.0)/2.0));
            directions.push_back(glm::dvec2(-0.5, -std::sqrt(3.0)/2.0));
            directions.push_back(glm::dvec2(0.0, 1.0));
            break;

        default:
            // Default to evenly spaced directions
            for (int i = 0; i < bondCount; ++i) {
                double angle = i * 2.0 * M_PI / bondCount;
                directions.push_back(glm::dvec2(std::cos(angle), std::sin(angle)));
            }
            break;
        }

        return directions;
    }

    std::vector<Atom::ElectronPair> Atom::GetLonePairs() const
    {
        std::vector<ElectronPair> lonePairs;
        for (const auto& pair : m_electronPairs) {
            if (pair.type == ElectronPairType::LonePair) {
                lonePairs.push_back(pair);
            }
        }
        return lonePairs;
    }

    Atom::MolecularGeometry Atom::GetMolecularGeometry() const
    {
        int bondCount = m_bondedAtoms.size();
        int electronPairs = bondCount; // Simplified - not counting lone pairs

        switch (electronPairs) {
        case 2: return MolecularGeometry::Linear;
        case 3: return MolecularGeometry::Trigonal;
        case 4: return MolecularGeometry::Tetrahedral;
        case 5: return MolecularGeometry::TrigonalBipyramidal;
        case 6: return MolecularGeometry::Octahedral;
        default: return MolecularGeometry::Tetrahedral;
        }
    }

    void Atom::UpdateElectronPairGeometry()
    {
        m_electronPairs.clear();

        // Add bonding pairs first
        for (Atom* bondedAtom : m_bondedAtoms) {
            glm::dvec2 bondDirection = glm::normalize(bondedAtom->GetPositionD() - m_position);
            m_electronPairs.emplace_back(ElectronPairType::BondingPair, bondDirection, bondedAtom);
        }

        // Calculate and add lone pairs
        int lonePairCount = GetLonePairCount();
        if (lonePairCount > 0) {
            int totalPairs = m_bondedAtoms.size() + lonePairCount;

            // Generate ideal directions for all electron pairs based on VSEPR theory
            std::vector<glm::dvec2> idealDirections;

            switch (totalPairs) {
                case 2: // Linear
                    idealDirections.emplace_back(1.0, 0.0);
                    idealDirections.emplace_back(-1.0, 0.0);
                    break;
                case 3: // Trigonal planar
                    for (int i = 0; i < 3; ++i) {
                        double angle = i * 2.0 * M_PI / 3.0;
                        idealDirections.emplace_back(std::cos(angle), std::sin(angle));
                    }
                    break;
                case 4: // Tetrahedral (2D approximation)
                    idealDirections.emplace_back(1.0, 0.0);
                    idealDirections.emplace_back(-0.5, std::sqrt(3.0)/2.0);
                    idealDirections.emplace_back(-0.5, -std::sqrt(3.0)/2.0);
                    idealDirections.emplace_back(0.0, 1.0);
                    break;
                default: // Evenly distributed
                    for (int i = 0; i < totalPairs; ++i) {
                        double angle = i * 2.0 * M_PI / totalPairs;
                        idealDirections.emplace_back(std::cos(angle), std::sin(angle));
                    }
                    break;
            }

            // Find unoccupied directions for lone pairs
            std::vector<glm::dvec2> availableDirections;

            for (const auto& idealDir : idealDirections) {
                bool isOccupied = false;

                // Check if this direction is close to any existing bonding pair
                for (const auto& pair : m_electronPairs) {
                    if (pair.type == ElectronPairType::BondingPair) {
                        double dot = glm::dot(idealDir, pair.direction);
                        if (dot > 0.8) { // Threshold for "close enough"
                            isOccupied = true;
                            break;
                        }
                    }
                }

                if (!isOccupied) {
                    availableDirections.push_back(idealDir);
                }
            }

            // Add lone pairs using available directions
            int lonePairsAdded = 0;
            for (const auto& direction : availableDirections) {
                if (lonePairsAdded >= lonePairCount) break;

                m_electronPairs.emplace_back(ElectronPairType::LonePair, direction);
                lonePairsAdded++;
            }

            // If we couldn't find enough ideal directions, distribute remaining lone pairs
            // to minimize repulsion
            while (lonePairsAdded < lonePairCount) {
                glm::dvec2 bestDirection(1.0, 0.0);
                double maxMinDistance = -1.0;

                // Test directions at 15-degree intervals
                for (int angle = 0; angle < 360; angle += 15) {
                    double rad = angle * M_PI / 180.0;
                    glm::dvec2 testDir(std::cos(rad), std::sin(rad));

                    // Find minimum distance to existing electron pairs
                    double minDistance = std::numeric_limits<double>::max();
                    for (const auto& pair : m_electronPairs) {
                        double distance = glm::length(testDir - pair.direction);
                        minDistance = std::min(minDistance, distance);
                    }

                    if (minDistance > maxMinDistance) {
                        maxMinDistance = minDistance;
                        bestDirection = testDir;
                    }
                }

                m_electronPairs.emplace_back(ElectronPairType::LonePair, bestDirection);
                lonePairsAdded++;
            }
        }
    }
}
