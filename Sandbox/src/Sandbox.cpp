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
		MOL_INFO("Example Layer Handeled");
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
	}
	~Sandbox() 
	{
	}
};

Molecular::App* Molecular::CreateApplication()
{
	return new Sandbox();
}