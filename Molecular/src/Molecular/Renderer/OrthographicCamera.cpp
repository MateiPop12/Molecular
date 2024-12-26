#include "OrthographicCamera.h"

#include <gtc/matrix_transform.hpp>

namespace Molecular
{
    OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
        : m_ProjectionMatrix(glm::ortho(left, right, bottom, top, -1.0f, 1.0f)), m_ViewMatrix(1.0f)
    {
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }

    void OrthographicCamera::recalculateViewMatrix()
    {
        glm::mat4 transform = translate(glm::mat4(1.0f),m_Position) *
            rotate(glm::mat4(1.0f), m_Rotation, glm::vec3(0, 0, 1));

        m_ViewMatrix = inverse(transform);
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }
}
