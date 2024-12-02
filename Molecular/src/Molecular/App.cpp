#include "App.h"
#include "Log.h"


#include <GLFW/glfw3.h>

namespace Molecular
{
	App::App()
	{
		m_window = std::unique_ptr<Window>(Window::Create(WindowProps()));
		m_window->SetEventCallback(std::bind(&App::OnEvent, this, std::placeholders::_1));
	}
	App::~App()
	{
	}

	void App::OnEvent(Event &e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(std::bind(&App::OnWindowClose, this, std::placeholders::_1));
		//dispatcher.Dispatch<WindowResizeEvent>(std::bind(&App::OnWindowResize, this, std::placeholders::_1));
		MOL_CORE_TRACE("{0}",e);
	}

	void App::Run() 
	{
		while(m_running)
		{
			glClearColor(1.0f, 0.5f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			m_window->OnUpdate();
		}
	}

	bool App::OnWindowClose(WindowCloseEvent& e)
	{
		m_running = false;
		return true;
	}
}
