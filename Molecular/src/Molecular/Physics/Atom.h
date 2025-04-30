#pragma once

#include "AtomData.h"
#include <glm.hpp>

namespace Molecular{
    class Atom{
    public:
        Atom(glm::dvec2 position, glm::dvec2 velocity, double mass,
             double vanDerWaalsRadius, double bondLength, double epsilon, double sigma, double electronegativity, int valence, glm::vec4 color)
                : m_position(position), m_velocity(velocity), m_mass(mass),
                  m_vanDerWaalsRadius(vanDerWaalsRadius), m_CovalentBondLength(bondLength),
                  m_epsilon(epsilon), m_sigma(sigma),m_electronegativity(electronegativity), m_valence(valence), m_color(color) {}

        Atom(const std::string& element, glm::dvec2 position, glm::dvec2 velocity = {0.0f, 0.0f})
        {
            if (elementData.find(element) != elementData.end()) {
                const auto& data = elementData[element];
                m_mass = data.mass;
                m_vanDerWaalsRadius = data.vanDerWaalsRadius;
                m_CovalentBondLength = data.bondLength;
                m_epsilon = data.epsilon;
                m_sigma = data.sigma;
                m_electronegativity = data.electronegativity;
                m_valence = data.valence;
                m_color = data.color;
            } else {
                throw std::runtime_error("Unknown element type: " + element);
            }
            m_element = element;
            m_position = position;
            m_velocity = velocity;
        }

        // === Floating-point ===
        glm::vec2 GetPosition() const { return glm::vec2(m_position); }
        glm::vec2 GetVelocity() const { return glm::vec2(m_velocity); }

        void SetPosition(const glm::vec2& position) { m_position = glm::dvec2(position); }
        void SetVelocity(const glm::vec2& velocity) { m_velocity = glm::dvec2(velocity); }

        float GetVanDerWaalsRadius() const { return static_cast<float>(m_vanDerWaalsRadius); }

        // === Double-precision ===
        glm::dvec2 GetPositionD() const { return m_position; }
        glm::dvec2 GetVelocityD() const { return m_velocity; }

        void AddBond(Atom* other);

        void SetPosition(const glm::dvec2& position) { m_position = position; }
        void SetVelocity(const glm::dvec2& velocity) { m_velocity = velocity; }

        double GetMassD() const { return m_mass; }
        double GetVanDerWaalsRadiusD() const { return m_vanDerWaalsRadius; }
        double GetCovalentBondLengthD() const { return m_CovalentBondLength; }
        double GetEpsilonD() const { return m_epsilon; }
        double GetSigmaD() const { return m_sigma; }
        double GetElectronegativity() const { return m_electronegativity; }

        int GetValence() const { return m_valence; }
        int GetBondCount() const { return m_bond_count; }

        bool CanFormBond() const { return m_bondedAtoms.size() < static_cast<size_t>(m_valence);}
        bool CanBondWith(const Atom* other) const;
        bool IsBondedTo(const Atom* other) const;

        glm::vec4 GetColor() const { return m_color; }

        std::string GetElement() const { return m_element;}

        std::vector<Atom*>& GetBondedAtoms() { return m_bondedAtoms; }
        
    private:
        glm::dvec2 m_position;
        glm::dvec2 m_velocity;

        glm::vec4 m_color;

        double m_mass;                  // Atomic mass (amu)
        double m_vanDerWaalsRadius;     // Non-bonded interaction distance (nm)
        double m_CovalentBondLength;    // Bonding interaction distance (nm)
        double m_epsilon;               // Interaction strength (eV)
        double m_sigma;                 // Distance where potential is zero (nm)
        double m_electronegativity;     // Bonding preference
        unsigned int m_valence;         // Max number of bonds allowed (valence rule)
        unsigned int m_bond_count;      // Current number of bonds

        std::vector<Atom*> m_bondedAtoms;
        std::string m_element;
    };
}
