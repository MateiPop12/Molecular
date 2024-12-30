#include "OpenGLShader.h"

#include "Log.h"
#include "glad/glad.h"
#include <gtc/type_ptr.hpp>

namespace Molecular
{
	static GLenum ShaderTypeForm(const std::string& type)
	{
		if (type == "vertex")
			return GL_VERTEX_SHADER;
		if (type == "fragment")
			return GL_FRAGMENT_SHADER;
		MOL_CORE_ASSERT(false, "Unknown shader type '" + type + "'");
		return 0;
	}

    OpenGLShader::OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc)
    {
		std::unordered_map<GLenum, std::string> sources;
		sources[GL_VERTEX_SHADER] = vertexSrc;
		sources[GL_FRAGMENT_SHADER] = fragmentSrc;
		Compile(sources);
    }

    OpenGLShader::OpenGLShader(const std::string& path)
    {
    	std::string source = ReadFile(path);
		auto shaderSources = PreProcess(source);
		Compile(shaderSources);
    }

    OpenGLShader::~OpenGLShader()
    {
    	glDeleteProgram(m_RendererID);
    }

    void OpenGLShader::Bind() const
    {
    	glUseProgram(m_RendererID);
    }

    void OpenGLShader::Unbind() const
    {
    	glUseProgram(0);
    }

    void OpenGLShader::UploadUniformInt(const std::string& name, int value) const
    {
    	GLint location = glGetUniformLocation(m_RendererID, name.c_str());
    	glUniform1i(location, value);
    }

    void OpenGLShader::UploadUniformFloat(const std::string& name, const float value) const
    {
    	GLint location = glGetUniformLocation(m_RendererID, name.c_str());
    	glUniform1f(location, value);
    }

    void OpenGLShader::UploadUniformFloat2(const std::string& name, const glm::vec2& vector) const
    {
    	GLint location = glGetUniformLocation(m_RendererID, name.c_str());
    	glUniform2f(location, vector.x, vector.y);
    }

    void OpenGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& vector) const
    {
    	GLint location = glGetUniformLocation(m_RendererID, name.c_str());
    	glUniform3f(location, vector.x, vector.y, vector.z);
    }

	void OpenGLShader::UploadUniformFloat4(const std::string& name, const glm::vec4& matrix) const
    {
    	GLint location = glGetUniformLocation(m_RendererID, name.c_str());
    	glUniform4f(location, matrix.x, matrix.y, matrix.z, matrix.w );
    }

    void OpenGLShader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix) const
    {
    	GLint location = glGetUniformLocation(m_RendererID, name.c_str());
    	glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
    }

    void OpenGLShader::UploadUniformMat4(const std::string& name,const glm::mat4& matrix) const
    {
    	GLint location = glGetUniformLocation(m_RendererID, name.c_str());
    	glUniformMatrix4fv(location,1,GL_FALSE, value_ptr(matrix));
    }

    std::string OpenGLShader::ReadFile(const std::string& path)
    {
    	std::string source;
    	std::ifstream in(path,std::ios::in, std::ios::binary);

		if (in)
		{
			in.seekg(0, std::ios::end);
			size_t size = in.tellg();
			if (size != -1)
			{
				source.resize(size);
				in.seekg(0, std::ios::beg);
				in.read(&source[0], size);
			}
			else
			{
				MOL_CORE_ERROR("Could not read from file '{0}'", source);
			}
		}
		else
		{
			MOL_CORE_ERROR("Could not open file '{0}'", source);
		}

    	return source;
    }

    std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string& path)
    {
		std::unordered_map<GLenum, std::string> shaderSources;

		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = path.find(typeToken, 0);
		while (pos != std::string::npos)
		{
			size_t eol = path.find_first_of("\r\n", pos);
			MOL_CORE_ASSERT(eol != std::string::npos, "Syntax error");
			size_t begin = pos + typeTokenLength + 1;
			std::string type = path.substr(begin, eol - begin);
			MOL_CORE_ASSERT(ShaderTypeForm(type), "Invalid shader type specified");

			size_t nextLinePos = path.find_first_not_of("\r\n", eol);
			MOL_CORE_ASSERT(nextLinePos != std::string::npos, "Syntax error");
			pos = path.find(typeToken, nextLinePos);

			shaderSources[ShaderTypeForm(type)] = pos == std::string::npos ? path.substr(nextLinePos) : path.substr(nextLinePos, pos - nextLinePos);
		}

		return shaderSources;
    }

    void OpenGLShader::Compile(const std::unordered_map<GLenum, std::string>& shaderSources)
    {
		GLuint program = glCreateProgram();
		std::vector<GLenum> glShaderIDs(shaderSources.size());

		for (auto& shaderSource : shaderSources)
		{
			GLenum type = shaderSource.first;
			const std::string& source = shaderSource.second;

			GLuint shader = glCreateShader(type);

			const GLchar *sourceCStr = source.c_str();
			glShaderSource(shader, 1, &sourceCStr, 0);

			glCompileShader(shader);

			GLint isCompiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
			if(isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

				glDeleteShader(shader);

				MOL_CORE_ERROR("{0}", infoLog.data());
				MOL_CORE_ASSERT(false,"Shader compilation failed!");
				break;
			}
			glAttachShader(program, shader);
			glShaderIDs.push_back(shader);
		}

		glLinkProgram(program);

		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

			glDeleteProgram(program);
			for (auto shaderID : glShaderIDs)
				glDeleteShader(shaderID);


			MOL_CORE_ERROR("{0}", infoLog.data());
			MOL_CORE_ASSERT(false,"Shader linking has failed!");
			return;
		}

		for (auto shaderID : glShaderIDs)
    		glDetachShader(program, shaderID);

		m_RendererID = program;
    }
}
