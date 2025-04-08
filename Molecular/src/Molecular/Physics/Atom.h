#pragma once

#include <glm.hpp>

namespace Molecular{

    struct AtomProperties {
        double mass;
        double vanDerWaalsRadius;
        double bondLength;
        double epsilon;
        double sigma;
        glm::vec4 color;
    };

    static inline std::unordered_map<std::string, AtomProperties> elementData = {
            {"H",  {1.008, 0.12, 0.074, 0.028, 0.2958, glm::vec4(1.0f, 0.0f, 1.0f, 1.0f)}},
            {"O",  {15.999, 0.152, 0.121, 0.095, 0.3165, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)}},
            {"C",  {12.011, 0.17, 0.154, 0.12, 0.34, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)}},
            {"N",  {14.007, 0.155, 0.145, 0.07, 0.325, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)}},
    };

    static inline std::unordered_map<std::string, int> maxBonds = {
            {"H", 1},
            {"O", 2},
            {"N", 3},
            {"C", 4}
    };

    class Atom{
    public:
        Atom(glm::dvec2 position, glm::dvec2 velocity, double mass,
             double vanDerWaalsRadius, double bondLength, double epsilon, double sigma, glm::vec4 color)
                : m_position(position), m_velocity(velocity), m_mass(mass),
                  m_vanDerWaalsRadius(vanDerWaalsRadius), m_CovalentBondLength(bondLength),
                  m_epsilon(epsilon), m_sigma(sigma), m_color(color) {}

        Atom(const std::string& element, glm::dvec2 position, glm::dvec2 velocity = {0.0f, 0.0f})
        {
            if (elementData.find(element) != elementData.end()) {
                const auto& data = elementData[element];
                m_mass = data.mass;
                m_vanDerWaalsRadius = data.vanDerWaalsRadius;
                m_CovalentBondLength = data.bondLength;
                m_epsilon = data.epsilon;
                m_sigma = data.sigma;
                m_color = data.color;
            } else {
                throw std::runtime_error("Unknown element type: " + element);
            }
            m_element = element;
            m_position = position;
            m_velocity = velocity;
        }

        // === Double-precision ===
        glm::dvec2 GetPositionD() const { return m_position; }
        glm::dvec2 GetVelocityD() const { return m_velocity; }
        void SetPosition(const glm::dvec2& position) { m_position = position; }
        void SetVelocity(const glm::dvec2& velocity) { m_velocity = velocity; }
        double GetMassD() const { return m_mass; }
        double GetVanDerWaalsRadiusD() const { return m_vanDerWaalsRadius; }
        double GetCovalentBondLengthD() const { return m_CovalentBondLength; }
        double GetEpsilonD() const { return m_epsilon; }
        double GetSigmaD() const { return m_sigma; }
        glm::vec4 GetColor() const { return m_color; }
        std::string GetElement() const { return m_element;}

        // === Floating-point ===
        glm::vec2 GetPosition() const { return glm::vec2(m_position); }
        glm::vec2 GetVelocity() const { return glm::vec2(m_velocity); }
        void SetPosition(const glm::vec2& position) { m_position = glm::dvec2(position); }
        void SetVelocity(const glm::vec2& velocity) { m_velocity = glm::dvec2(velocity); }
        float GetVanDerWaalsRadius() const { return static_cast<float>(m_vanDerWaalsRadius); }

        size_t GetBondCount() const { return m_bondedAtoms.size(); }

        bool IsBondedTo(Atom* other);
        void AddBond(Atom* other);
        
    private:
        glm::dvec2 m_position;
        glm::dvec2 m_velocity;

        glm::vec4 m_color;

        double m_mass;                 // Atomic mass (amu)
        double m_vanDerWaalsRadius;    // Non-bonded interaction distance (nm)
        double m_CovalentBondLength;   // Bonding interaction distance (nm)
        double m_epsilon;              // Interaction strength (eV)
        double m_sigma;                // Distance where potential is zero (nm)

        std::vector<Atom*> m_bondedAtoms;
        std::string m_element;
    };
}
