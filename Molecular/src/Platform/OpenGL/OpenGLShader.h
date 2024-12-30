#pragma once

#include "Molecular/Renderer/Shader.h"
#include <glm.hpp>

typedef unsigned int GLenum;

namespace Molecular
{
    class OpenGLShader : public Shader
    {
    public:
        OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc);
        OpenGLShader(const std::string& path);

        ~OpenGLShader();

        void Bind() const override;
        void Unbind() const override;

        void UploadUniformInt(const std::string& name, int value) const;

        void UploadUniformFloat(const std::string& name, float value) const;
        void UploadUniformFloat2(const std::string& name, const glm::vec2& vector) const;
        void UploadUniformFloat3(const std::string& name, const glm::vec3& vector) const;
        void UploadUniformFloat4(const std::string& name, const glm::vec4& vector) const;

        void UploadUniformMat3(const std::string& name, const glm::mat3& matrix) const;
        void UploadUniformMat4(const std::string& name, const glm::mat4& matrix) const;

    private:
        std::string ReadFile(const std::string& path);
        std::unordered_map<GLenum, std::string> PreProcess(const std::string& path);
        void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);
        uint32_t m_RendererID;
    };
}
