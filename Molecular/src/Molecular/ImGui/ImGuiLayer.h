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
        //void OnEvent(Event& e) override;
        void OnImGuiRender() override;

        static void Begin();
        static void End();
        // void BlockEvents(bool block) { m_BlockEvents = block; }
        // void SetDarkThemeColors();

        //uint32_t GetActiveWidgetID() const;
    private:
        bool m_BlockEvents = true;
    };

}