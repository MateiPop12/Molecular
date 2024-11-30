// #include "molpch.h"
#include "App.h"
//#include <GLFW/glfw3.h>

namespace Molecular
{
	App::App()
	{
		//m_window = std::unique_ptr<Window>(Window::Create(WindowProps()));
	}
	App::~App()
	{
	}
	void App::Run() 
	{
		while(m_running)
		{
			/*glClearColor(1.0f, 0.5f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			m_window->OnUpdate();*/
		}
	}
}
