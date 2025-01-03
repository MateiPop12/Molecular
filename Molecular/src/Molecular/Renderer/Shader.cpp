#include "molpch.h"
#include "Molecular/Renderer/Shader.h"

#include "Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace Molecular
{
	Ref<Shader> Shader::Create(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None      :MOL_CORE_ASSERT(false,"RendererAPI::None not supported"); return nullptr;
		case RendererAPI::API::OpenGL    :return std::make_shared<OpenGLShader>(name,vertexPath, fragmentPath);
		}
		MOL_CORE_ASSERT(false,"Unknown RendererAPI");
		return nullptr;
	}

	Ref<Shader> Shader::Create(const std::string& path)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None      :MOL_CORE_ASSERT(false,"RendererAPI::None not supported"); return nullptr;
		case RendererAPI::API::OpenGL    :return std::make_shared<OpenGLShader>(path);
		}
		MOL_CORE_ASSERT(false,"Unknown RendererAPI");
		return nullptr;
	}

	void ShaderLibrary::Add(const Ref<Shader>& shader)
	{
		auto& name = shader->GetName();
		Add(name, shader);
	}

	void ShaderLibrary::Add(const std::string& name,const Ref<Shader>& shader)
	{
		MOL_CORE_ASSERT(m_shaders.find(name) == m_shaders.end(), "Shader already exists");
		m_shaders[name] = shader;
	}

	Ref<Shader> ShaderLibrary::Load(const std::string& path)
	{
		auto shader = Shader::Create(path);
		Add(shader);
		return shader;
	}

	Ref<Shader> ShaderLibrary::Load(const std::string& name, const std::string& path)
	{
		auto shader = Shader::Create(path);
		Add(name, shader);
		return shader;
	}

	Ref<Shader> ShaderLibrary::Get(const std::string& name)
	{
		MOL_CORE_ASSERT(m_shaders.find(name) != m_shaders.end(), "Shader not found");
		return m_shaders[name];
	}
}
