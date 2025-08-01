#include "../../molpch.h"
#include "App.h"

#include "../../../vendor/imgui/imgui.h"
#include "Log.h"
#include "../../../vendor/glfw/include/GLFW/glfw3.h"
#include "Core.h"

#include "../Renderer/RenderCommand.h"
#include "../Renderer/Renderer.h"

#include "../Renderer/OrthographicCamera.h"

namespace Molecular
{
	App* App::s_Instance = nullptr;

	App::App()
	{
		MOL_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		m_window = Scope<Window>(Window::Create(WindowProps()));
		m_window->SetEventCallback(MOL_BIND_EVENT_FN(App::OnEvent));
		m_window->SetVSync(false);

		// DEBUG: Log initial window size
		MOL_CORE_INFO("Initial window size: {}x{}", m_window->GetWidth(), m_window->GetHeight());

		Renderer::Init();

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);

		// DEBUG: Force initial resize event after all layers are attached
		WindowResizeEvent initialResize(m_window->GetWidth(), m_window->GetHeight());
		MOL_CORE_INFO("Dispatching initial resize event: {}x{}", m_window->GetWidth(), m_window->GetHeight());
		OnEvent(initialResize);
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
		dispatcher.Dispatch<WindowCloseEvent>(MOL_BIND_EVENT_FN(App::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(MOL_BIND_EVENT_FN(App::OnWindowResize));

		for (auto it = m_layerStack.end(); it != m_layerStack.begin(); )
		{
			(*--it)->OnEvent(e);
			if (e.Handled)
				break;
		}
	}

	void App::Run() 
	{
        const float fixedTimestep = 0.001f;
        float accumulator = 0.0f;

		while(m_running)
		{
			float time = (float)glfwGetTime();
			Timestep timestep = time - m_lastFrameTime;
			m_lastFrameTime = time;

            accumulator += timestep.GetSeconds();

            while (accumulator >= fixedTimestep)
            {
                if (!m_minimized)
                {
                    for (Layer* layer : m_layerStack)
                        layer->OnUpdate(fixedTimestep);
                }
                accumulator -= fixedTimestep;
            }

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

	bool App::OnWindowResize(WindowResizeEvent& e)
	{
		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_minimized = true;
			return false;
		}

		m_minimized = false;
		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

		return false;
	}
}
