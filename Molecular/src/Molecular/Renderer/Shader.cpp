#include "molpch.h"
#include "Molecular/Renderer/Shader.h"

#include "Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace Molecular
{
	Shader* Shader::Create(const std::string& vertexPath, const std::string& fragmentPath)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None      :MOL_CORE_ASSERT(false,"RendererAPI::None not supported"); return nullptr;
		case RendererAPI::API::OpenGL    :return new OpenGLShader(vertexPath, fragmentPath);
		}
		MOL_CORE_ASSERT(false,"Unknown RendererAPI");
		return nullptr;
	}
}
