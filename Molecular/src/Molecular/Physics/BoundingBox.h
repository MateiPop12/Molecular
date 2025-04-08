#pragma once

#include "glm.hpp"

namespace Molecular {
    class BoundingBox {
    public:
        BoundingBox(glm::dvec2 minPoint, glm::dvec2 maxPoint)
                : m_minPoint(minPoint), m_maxPoint(maxPoint) {}

        bool IsInside(const glm::vec2 &position) const {
            return position.x >= m_minPoint.x && position.x <= m_maxPoint.x &&
                   position.y >= m_minPoint.y && position.y <= m_maxPoint.y;
        }

        glm::dvec2 GetMinPoint() const { return m_minPoint; }
        glm::dvec2 GetMaxPoint() const { return m_maxPoint; }

    private:
        glm::dvec2 m_minPoint;  // Lower-left corner of the bounding box
        glm::dvec2 m_maxPoint;  // Upper-right corner of the bounding box
    };
}