#include <ext/matrix_transform.hpp>

#include "Molecular.h"

class ExampleLayer : public Molecular::Layer
{
public:
	ExampleLayer()
		:Layer("Example Layer"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f), m_CameraPosition(0.0f), m_SquarePosition(0.0f)
	{
		m_VertexArray.reset(Molecular::VertexArray::Create());

		float vertices[3*7] = {
			-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
			-0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		};

		std::shared_ptr<Molecular::VertexBuffer> vertexBuffer;
		vertexBuffer.reset(Molecular::VertexBuffer::Create(vertices,sizeof(vertices)));
		Molecular::BufferLayout layout = {
			{ Molecular::ShaderDataType::Float3, "a_Position" },
			{ Molecular::ShaderDataType::Float4, "a_Color"}
		};

		vertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		uint32_t indices[3] = { 0, 1, 2 };
		std::shared_ptr<Molecular::IndexBuffer> indexBuffer;
		indexBuffer.reset(Molecular::IndexBuffer::Create(indices,3));
		m_VertexArray->SetIndexBuffer(indexBuffer);

		m_SquareVertexArray.reset(Molecular::VertexArray::Create());
		float vertices2[3*4] = {
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.5f,  0.5f, 0.0f,
			-0.5f,  0.5f, 0.0f
		};
		std::shared_ptr<Molecular::VertexBuffer> squareVB;
		squareVB.reset(Molecular::VertexBuffer::Create(vertices2,sizeof(vertices2)));
		squareVB->SetLayout({
			{ Molecular::ShaderDataType::Float3, "a_Position" }
			});
		m_SquareVertexArray->AddVertexBuffer(squareVB);

		uint32_t indices2[6] = { 0, 1, 2, 2, 3, 0 };
		std::shared_ptr<Molecular::IndexBuffer> squareIB;
		squareIB.reset(Molecular::IndexBuffer::Create(indices2,6));
		m_SquareVertexArray->SetIndexBuffer(squareIB);

		std::string vertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			uniform mat4  u_ViewProjection;
			uniform mat4  u_Transform;

			out vec3 v_Position;
			out vec4 v_Color;

			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
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

		m_Shader.reset(new Molecular::Shader(vertexSrc, fragmentSrc));

		std::string rgbShaderVertexSrc= R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;

			uniform mat4  u_ViewProjection;
			uniform mat4  u_Transform;

			out vec3 v_Position;

			void main()
			{
				v_Position = a_Position;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
			}
		)";

		std::string rgbShaderFragmentSrc = R"(
			#version 330 core

			layout(location = 0) out vec4 color;

			in vec3 v_Position;

			uniform vec4 u_Color;

			void main()
			{
				color = u_Color;
			}
		)";

		m_rgbShader.reset(new Molecular::Shader(rgbShaderVertexSrc, rgbShaderFragmentSrc));
	}

	void OnUpdate(Molecular::Timestep timestep) override
	{
		if (Molecular::Input::IsKeyPressed(Molecular::Key::Left))
			m_CameraPosition.x -= m_CameraMoveSpeed * timestep.getSeconds();
		else if (Molecular::Input::IsKeyPressed(Molecular::Key::Right))
			m_CameraPosition.x += m_CameraMoveSpeed * timestep.getSeconds();

		if (Molecular::Input::IsKeyPressed(Molecular::Key::Down))
			m_CameraPosition.y -= m_CameraMoveSpeed * timestep.getSeconds();
		else if (Molecular::Input::IsKeyPressed(Molecular::Key::Up))
			m_CameraPosition.y += m_CameraMoveSpeed * timestep.getSeconds();

		if (Molecular::Input::IsKeyPressed(Molecular::Key::Z))
			m_CameraRotation += m_CameraRotationSpeed * timestep.getSeconds();
		else if (Molecular::Input::IsKeyPressed(Molecular::Key::X))
			m_CameraRotation -= m_CameraRotationSpeed * timestep.getSeconds();

		Molecular::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.0f});
		Molecular::RenderCommand::Clear();

		m_Camera.setPosition(m_CameraPosition);
		m_Camera.setRotation(m_CameraRotation);

		Molecular::Renderer::BeginScene(m_Camera);

		const glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
		glm::vec4 red = glm::vec4(1.0f, 0.0f, 0.0f,1.0f);
		glm::vec4 green = glm::vec4(0.0f, 1.0f, 0.0f,1.0f);
		glm::vec4 blue = glm::vec4(0.0f, 0.0f, 1.0f,1.0f);

		int i = 0;
		for (int y = 0; y < 20; y++)
		{
			for (int x = 0; x < 20 ; x++)
			{
				glm::vec3 pos(x * 0.11f, y * 0.11f,0.0f);
				glm::mat4 transform = translate(glm::mat4(1.0f), pos) * scale;
				if (i % 3 == 0)
					m_rgbShader->UploadUniformFloat4("u_Color", red);
				else if (i%3 == 1)
					m_rgbShader->UploadUniformFloat4("u_Color", green);
				else if (i%3 == 2)
					m_rgbShader->UploadUniformFloat4("u_Color", blue);
				Molecular::Renderer::Submit(m_SquareVertexArray,m_rgbShader, transform);
				i++;
			}
		}

		Molecular::Renderer::Submit(m_VertexArray,m_Shader);

		Molecular::Renderer::EndScene();
	}

	void OnImGuiRender() override
	{
	}

	void OnEvent(Molecular::Event& event) override
	{
	}

private:
	std::shared_ptr<Molecular::Shader>		m_Shader;
	std::shared_ptr<Molecular::VertexArray>	m_VertexArray;

	std::shared_ptr<Molecular::VertexArray>	m_SquareVertexArray;
	std::shared_ptr<Molecular::Shader>		m_rgbShader;

	Molecular::OrthographicCamera m_Camera;
	glm::vec3 m_CameraPosition;
	float m_CameraMoveSpeed = 1.0f;
	float m_CameraRotation = 0.0f;
	float m_CameraRotationSpeed = 0.1f;

	glm::vec3 m_SquarePosition;
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