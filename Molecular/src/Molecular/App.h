#pragma once

#include "Core.h"
#include "Window.h"
#include "LayerStack.h"
#include "Events/Event.h"
#include "Events/ApplicationEvent.h"
#include "ImGui/ImGuiLayer.h"

#include "Molecular/Renderer/Shader.h"
#include "Molecular/Renderer/Buffer.h"
#include "Molecular/Renderer/VertexArray.h"
#include "Renderer/OrthographicCamera.h"

namespace Molecular
{
	class MOLECULAR_API App
	{
	public:
		App();
		virtual ~App();

		void Run();
		void OnEvent(Event &e);
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		Window& GetWindow() { return *m_window; }
		static App& Get()	{ return *s_Instance;}

	private:
		bool OnWindowClose(WindowCloseEvent& e);
		std::unique_ptr<Window> m_window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_running = true;
		LayerStack m_layerStack;

		std::shared_ptr<Shader>			m_Shader;
		std::shared_ptr<VertexArray>	m_VertexArray;

		std::shared_ptr<VertexArray>	m_SquareVertexArray;
		std::shared_ptr<Shader>			m_Shader2;

		OrthographicCamera m_Camera;

		static App* s_Instance;
	};

	App* CreateApplication();
}


