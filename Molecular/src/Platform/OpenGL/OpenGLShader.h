#pragma once

#include "Molecular/Renderer/Shader.h"
#include <glm.hpp>

typedef unsigned int GLenum;

namespace Molecular
{
    class OpenGLShader : public Shader
    {
    public:
        OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
        OpenGLShader(const std::string& path);

        ~OpenGLShader();

        void Bind() const override;
        void Unbind() const override;

        const std::string GetName() const override {return m_name; }

        void UploadUniformInt(const std::string& name, int value) const;

        void UploadUniformFloat(const std::string& name, float value) const;
        void UploadUniformFloat2(const std::string& name, const glm::vec2& vector) const;
        void UploadUniformFloat3(const std::string& name, const glm::vec3& vector) const;
        void UploadUniformFloat4(const std::string& name, const glm::vec4& vector) const;

        void UploadUniformMat3(const std::string& name, const glm::mat3& matrix) const;
        void UploadUniformMat4(const std::string& name, const glm::mat4& matrix) const;

        void SetInt(const std::string& name, int value) override;
        void SetFloat(const std::string& name, float value) override;
        void SetFloat3(const std::string& name, const glm::vec3& value) override;
        void SetFloat4(const std::string& name, const glm::vec4& value) override;
        void SetMat4(const std::string& name, const glm::mat4& value) override;

    private:
        std::string ReadFile(const std::string& path);
        std::unordered_map<GLenum, std::string> PreProcess(const std::string& path);
        void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);
        uint32_t m_RendererID;
        std::string m_name;
    };
}
