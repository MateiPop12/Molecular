#pragma once
#include "Core/Timestep.h"
#include "Events/ApplicationEvent.h"
#include "Events/Event.h"
#include "Events/MouseEvent.h"
#include "Renderer/OrthographicCamera.h"

namespace Molecular
{
    class OrthographicCameraController
    {
    public:
        OrthographicCameraController(float aspectRatio, bool rotation = false);

        void OnUpdate(Timestep timestep);
        void OnEvent(Event& e);

        OrthographicCamera& GetCamera() {return m_camera;}
        const OrthographicCamera& GetCamera() const {return m_camera;}


    private:
        bool OnMouseScrolled(MouseScrolledEvent& e);
        bool OnWindowResized(WindowResizeEvent& e);

        float m_aspectRatio;
        float m_zoomLevel = 1.0f;
        bool m_rotation;

        float m_cameraRotation = 0.0f;
        float m_cameraTranslationSpeed = 1.0f;
        float m_cameraRotationSpeed = 1.0f;
        glm::vec3 m_cameraPosition = {0.0f,0.0f,0.0f};
        OrthographicCamera m_camera;
    };

}
