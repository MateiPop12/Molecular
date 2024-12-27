#include "App.h"

#include "imgui.h"
#include "Log.h"
#include "GLFW/glfw3.h"

#include "Renderer/RenderCommand.h"
#include "Renderer/Renderer.h"

#include "Renderer/OrthographicCamera.h"

namespace Molecular
{
	App* App::s_Instance = nullptr;

	App::App()
	{
		MOL_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		m_window = std::unique_ptr<Window>(Window::Create(WindowProps()));
		m_window->SetEventCallback(std::bind(&App::OnEvent, this, std::placeholders::_1));
		m_window->SetVSync(false);

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
	}
	App::~App()
	{
	}

	void App::PushLayer(Layer* layer)
	{
		m_layerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void App::PushOverlay(Layer* layer)
	{
		m_layerStack.PushOverlay(layer);
		layer->OnAttach();
	}

	void App::OnEvent(Event &e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(std::bind(&App::OnWindowClose, this, std::placeholders::_1));

		for (auto it = m_layerStack.end(); it != m_layerStack.begin(); )
		{
			(*--it)->OnEvent(e);
			if (e.Handled)
				break;
		}
	}

	void App::Run() 
	{
		while(m_running)
		{
			float time = (float)glfwGetTime();
			Timestep timestep = time - m_lastFrameTime;
			m_lastFrameTime = time;

			for (Layer* layer : m_layerStack)
				layer->OnUpdate(timestep);

			m_ImGuiLayer->Begin();
			for (Layer* layer : m_layerStack)
				layer->OnImGuiRender();
			m_ImGuiLayer->End();

			m_window->OnUpdate();
		}
	}

	bool App::OnWindowClose(WindowCloseEvent& e)
	{
		m_running = false;
		return true;
	}
}
