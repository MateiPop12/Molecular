#pragma once

#include "Core.h"
#include "Window.h"
#include "LayerStack.h"
#include "Events/Event.h"
#include "Events/ApplicationEvent.h"

namespace Molecular
{
	class MOLECULAR_API App
	{
	public:
		App();
		virtual ~App();

		void Run();
		void OnEvent(Event &e);
		void PushLayer(Layer *l);
		void PushOverlay(Layer *l);
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		std::unique_ptr<Window> m_window;
		bool m_running = true;
		LayerStack m_layerStack;
	};

	App* CreateApplication();
}


