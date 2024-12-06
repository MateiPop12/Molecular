#include "Molecular.h"

class ExampleLayer : public Molecular::Layer
{
	public:
	ExampleLayer()
		:Layer("Example Layer")
	{
	}

	void OnUpdate() override
	{
	}

	void OnEvent(Molecular::Event& event) override
	{
		MOL_TRACE("{0}",event);
	}
};

class Sandbox : public Molecular::App
{
public:
	Sandbox() 
	{
		PushLayer(new ExampleLayer());
		PushOverlay(new Molecular::ImGuiLayer());
	}
	~Sandbox() 
	{
	}
};

Molecular::App* Molecular::CreateApplication()
{
	return new Sandbox();
}