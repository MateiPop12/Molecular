#include "App.h"
#include "Log.h"

#include "glad/glad.h"
#include "Input.h"

namespace Molecular
{
	App* App::s_Instance = nullptr;

	App::App()
	{
		MOL_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		m_window = std::unique_ptr<Window>(Window::Create(WindowProps()));
		m_window->SetEventCallback(std::bind(&App::OnEvent, this, std::placeholders::_1));

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);

		m_VertexArray.reset(VertexArray::Create());

		float vertices[3*7] = {
			-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
			-0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		};

		std::shared_ptr<VertexBuffer> vertexBuffer;
		vertexBuffer.reset(VertexBuffer::Create(vertices,sizeof(vertices)));
		BufferLayout layout = {
			{ ShaderDataType::Float3, "a_Position" },
			{ShaderDataType::Float4, "a_Color"}
		};

		vertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		uint32_t indices[3] = { 0, 1, 2 };
		std::shared_ptr<IndexBuffer> indexBuffer;
		indexBuffer.reset(IndexBuffer::Create(indices,3));
		m_VertexArray->SetIndexBuffer(indexBuffer);

		m_SquareVertexArray.reset(VertexArray::Create());
		float vertices2[3*4] = {
			-0.75f, -0.75f, 0.0f,
			 0.75f, -0.75f, 0.0f,
			 0.75f,  0.75f, 0.0f,
			-0.75f,  0.75f, 0.0f
		};
		std::shared_ptr<VertexBuffer> squareVB;
		squareVB.reset(VertexBuffer::Create(vertices2,sizeof(vertices2)));
		squareVB->SetLayout({
			{ ShaderDataType::Float3, "a_Position" }
			});
		m_SquareVertexArray->AddVertexBuffer(squareVB);

		uint32_t indices2[6] = { 0, 1, 2, 2, 3, 0 };
		std::shared_ptr<IndexBuffer> squareIB;
		squareIB.reset(IndexBuffer::Create(indices2,6));
		m_SquareVertexArray->SetIndexBuffer(squareIB);

		std::string vertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			out vec3 v_Position;
			out vec4 v_Color;

			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = vec4(a_Position, 1.0);
			}
		)";

		std::string fragmentSrc = R"(
			#version 330 core

			layout(location = 0) out vec4 color;

			in vec3 v_Position;
			in vec4 v_Color;

			void main()
			{
				color = vec4(v_Position * 0.5 + 0.1, 1.0);
				color = v_Color;
			}
		)";

		m_Shader.reset(new Shader(vertexSrc, fragmentSrc));

		std::string vertexSrc2 = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;

			out vec3 v_Position;

			void main()
			{
				v_Position = a_Position;
				gl_Position = vec4(a_Position, 1.0);
			}
		)";

		std::string fragmentSrc2 = R"(
			#version 330 core

			layout(location = 0) out vec4 color;

			in vec3 v_Position;

			void main()
			{
				color = vec4(0.2, 0.3, 0.8, 1.0);
			}
		)";

		m_Shader2.reset(new Shader(vertexSrc2, fragmentSrc2));
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
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			m_Shader2->Bind();
			m_SquareVertexArray->Bind();
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

			m_Shader->Bind();
			m_VertexArray->Bind();
			glDrawElements(GL_TRIANGLES, m_VertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);

			for (Layer* layer : m_layerStack)
				layer->OnUpdate();

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
