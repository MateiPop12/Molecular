#include <ext/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "Molecular.h"
#include "../../Molecular/vendor/imgui/imgui.h"
#include "Platform/OpenGL/OpenGLShader.h"

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
		float vertices2[5*4] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
		};
		std::shared_ptr<Molecular::VertexBuffer> squareVB;
		squareVB.reset(Molecular::VertexBuffer::Create(vertices2,sizeof(vertices2)));
		squareVB->SetLayout({
			{ Molecular::ShaderDataType::Float3, "a_Position" },
			{ Molecular::ShaderDataType::Float2, "a_TexCoord" }
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

		m_Shader.reset(Molecular::Shader::Create(vertexSrc, fragmentSrc));

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

			uniform vec3 u_Color;

			void main()
			{
				color = vec4(u_Color, 1.0);
			}
		)";

		m_rgbShader.reset(Molecular::Shader::Create(rgbShaderVertexSrc, rgbShaderFragmentSrc));

		std::string textureShaderVertexSrc= R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec2 a_TexCoord;

			uniform mat4  u_ViewProjection;
			uniform mat4  u_Transform;

			out vec2 v_TexCoord;

			void main()
			{
				v_TexCoord = a_TexCoord;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
			}
		)";

		std::string textureShaderFragmentSrc = R"(
			#version 330 core

			layout(location = 0) out vec4 color;

			in vec2 v_TexCoord;

			uniform sampler2D u_Texture;

			void main()
			{
				color = texture(u_Texture, v_TexCoord);
			}
		)";

		m_textureShader.reset(Molecular::Shader::Create(textureShaderVertexSrc, textureShaderFragmentSrc));

		m_Texture = Molecular::Texture2D::Create("D:/FACULTATE/Licenta/Molecular/Sandbox/assets/Checkerboard.png");
		m_ChernoLogoTexture = Molecular::Texture2D::Create("D:/FACULTATE/Licenta/Molecular/Sandbox/assets/ChernoLogo.png");


		std::dynamic_pointer_cast<Molecular::OpenGLShader>(m_textureShader)->Bind();
		std::dynamic_pointer_cast<Molecular::OpenGLShader>(m_textureShader)->UploadUniformInt("u_Texture", 0);
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

		std::dynamic_pointer_cast<Molecular::OpenGLShader>(m_rgbShader)->Bind();
		std::dynamic_pointer_cast<Molecular::OpenGLShader>(m_rgbShader)->UploadUniformFloat3("u_Color", m_SquareColor);

		for (int y = 0; y < 20; y++)
		{
			for (int x = 0; x < 20 ; x++)
			{
				glm::vec3 pos(x * 0.11f, y * 0.11f,0.0f);
				glm::mat4 transform = translate(glm::mat4(1.0f), pos) * scale;
				Molecular::Renderer::Submit(m_SquareVertexArray,m_rgbShader, transform);
			}
		}

		m_Texture->Bind();
		Molecular::Renderer::Submit(m_SquareVertexArray,m_textureShader);
		m_ChernoLogoTexture->Bind();
		Molecular::Renderer::Submit(m_SquareVertexArray,m_textureShader, translate(glm::mat4(1.0f), glm::vec3(-0.25f,-0.25f,0.0f)));

		//Molecular::Renderer::Submit(m_VertexArray,m_Shader);

		Molecular::Renderer::EndScene();
	}

	void OnImGuiRender() override
	{
		ImGui::Begin("Settings");
		ImGui::ColorEdit3("Square Color", value_ptr(m_SquareColor));
		ImGui::End();
	}

	void OnEvent(Molecular::Event& event) override
	{
	}

private:
	Molecular::Ref<Molecular::VertexArray>	m_VertexArray;
	Molecular::Ref<Molecular::VertexArray>	m_SquareVertexArray;

	Molecular::Ref<Molecular::Shader>	m_Shader;
	Molecular::Ref<Molecular::Shader>	m_rgbShader;
	Molecular::Ref<Molecular::Shader>	m_textureShader;

	Molecular::Ref<Molecular::Texture2D>m_Texture, m_ChernoLogoTexture;

	Molecular::OrthographicCamera m_Camera;
	glm::vec3 m_CameraPosition;
	float m_CameraMoveSpeed = 1.0f;
	float m_CameraRotation = 0.0f;
	float m_CameraRotationSpeed = 0.1f;

	glm::vec3 m_SquarePosition;

	glm::vec3 m_SquareColor = { 0.0f, 0.0f, 1.0f };
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