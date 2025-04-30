#pragma once

#include "molpch.h"
#include <glm.hpp>

namespace Molecular {
    struct AtomProperties {
        double mass;
        double vanDerWaalsRadius;
        double bondLength;
        double epsilon;
        double sigma;
        double electronegativity = 2.5;

        int valence;

        glm::vec4 color;
    };

    static inline std::unordered_map<std::string, AtomProperties> elementData = {
            {"H",  {1.008 , 0.12 , 0.074, 0.028, 0.2958, 2.2 , 1, glm::vec4(1.0f, 0.0f, 1.0f, 1.0f)}},
            {"O",  {15.999, 0.152, 0.121, 0.095, 0.3165, 3.44, 2, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)}},
            {"C",  {12.011, 0.17 , 0.154, 0.12 , 0.34  , 2.55, 4, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)}},
            {"N",  {14.007, 0.155, 0.145, 0.07 , 0.325 , 3.04, 2, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)}},
    };
}