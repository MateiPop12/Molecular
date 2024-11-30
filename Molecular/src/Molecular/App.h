#pragma once

#include <memory>

#include "Core.h"
//#include "Window.h"

namespace Molecular
{
	class MOLECULAR_API App
	{
	public:
		App();
		virtual ~App();

		void Run();
	private:
		//std::unique_ptr<Window> m_window;
		bool m_running = true;
	};

	App* CreateApplication();
}


