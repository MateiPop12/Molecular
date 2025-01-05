#include "OrthographicCameraController.h"

#include "../Core/Input.h"
#include "../Core/KeyCodes.h"

namespace Molecular
{
    OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool rotation)
        : m_aspectRatio(aspectRatio), m_camera(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel), m_rotation(rotation)
    {

    }

    void OrthographicCameraController::OnUpdate(Timestep timestep)
    {
        if (Input::IsKeyPressed(Key::Left))
            m_cameraPosition.x -= m_cameraTranslationSpeed * timestep.getSeconds();
        else if (Input::IsKeyPressed(Key::Right))
            m_cameraPosition.x += m_cameraTranslationSpeed * timestep.getSeconds();

        if (Input::IsKeyPressed(Key::Down))
            m_cameraPosition.y -= m_cameraTranslationSpeed * timestep.getSeconds();
        else if (Input::IsKeyPressed(Key::Up))
            m_cameraPosition.y += m_cameraTranslationSpeed * timestep.getSeconds();

        if (m_rotation)
        {
            if (Input::IsKeyPressed(Key::Z))
                m_cameraRotation += m_cameraRotationSpeed * timestep.getSeconds();
            else if (Input::IsKeyPressed(Key::X))
                m_cameraRotation -= m_cameraRotationSpeed * timestep.getSeconds();

            m_camera.setRotation(m_cameraRotation);

            m_cameraTranslationSpeed = m_zoomLevel;
        }

        m_camera.setPosition(m_cameraPosition);
    }

    void OrthographicCameraController::OnEvent(Event& e)
    {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<MouseScrolledEvent>(MOL_BIND_EVENT_FN(OrthographicCameraController::OnMouseScrolled));
        dispatcher.Dispatch<WindowResizeEvent>(MOL_BIND_EVENT_FN(OrthographicCameraController::OnWindowResized));

    }

    bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent& e)
    {
        m_zoomLevel -= e.GetYOffset() * 0.25f;
        m_zoomLevel = std::max(m_zoomLevel, 0.25f);
        m_camera.setProjection(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel);
        return false;
    }

    bool OrthographicCameraController::OnWindowResized(WindowResizeEvent& e)
    {
        m_aspectRatio = (float)e.GetWidth()/(float)e.GetHeight();
        m_camera.setProjection(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel);
        return false;
    }
}
