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
		if (Molecular::Input::IsKeyPressed(Molecular::Key::Space))
			MOL_INFO("Space");
	}

	// virtual void OnImGuiRender() override
	// {
	// 	ImGui::Begin("Test");;
	// 	ImGui::Text("Hallo!");
	// 	ImGui::End();
	// }

	void OnEvent(Molecular::Event& event) override
	{
		if (event.GetEventType() == Molecular::EventType::KeyPressed)
		{
			Molecular::KeyPressedEvent& e = (Molecular::KeyPressedEvent&)event;
			MOL_TRACE("{0}",(char)e.GetKeyCode());
		}
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