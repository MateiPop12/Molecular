#pragma once

#include "Molecular/Layer.h"

#include "Molecular/Events/KeyEvent.h"
#include "Molecular/Events/MouseEvent.h"
#include "Molecular/Events/ApplicationEvent.h"

namespace Molecular {

    class MOLECULAR_API ImGuiLayer final : public Layer
    {
    public:
        ImGuiLayer();
        ~ImGuiLayer() override;

        void OnAttach() override;
        void OnDetach() override;
        void OnEvent(Event& e) override;
        void OnUpdate() override;

        void Begin();
        void End();
        void BlockEvents(bool block) { m_BlockEvents = block; }
        void SetDarkThemeColors();

        uint32_t GetActiveWidgetID() const;
    private:
        bool m_BlockEvents = true;
        float m_Time = 0.0f;

        bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
        bool OnMouseButtonReleased(MouseButtonReleasedEvent& e);
        bool OnMouseMoved(MouseMovedEvent& e);
        bool OnMouseScrolled(MouseScrolledEvent& e);
        bool OnKeyPressed(KeyPressedEvent& e);
        bool OnKeyReleased(KeyReleasedEvent& e);
        bool OnWindowResized(WindowResizeEvent& e);
    };

}