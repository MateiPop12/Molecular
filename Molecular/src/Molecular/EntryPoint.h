#pragma once
#include "Log.h"

#ifdef MOL_PLATFORM_WINDOWS

extern Molecular::App* Molecular::CreateApplication();

int main(int argc, char** argv) {

	Molecular::Log::Init();
	MOL_CORE_INFO("Initialized Log!");

	auto app = Molecular::CreateApplication();
	app->Run();
	delete app;
}

#endif