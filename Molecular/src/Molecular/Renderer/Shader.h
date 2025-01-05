#pragma once
#include "../Core/Core.h"

namespace Molecular
{
    class Shader
    {
    public:
        virtual ~Shader() = default;

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;
        virtual const std::string GetName() const = 0;

        static Ref<Shader> Create(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath);
        static Ref<Shader> Create(const std::string& path);
    };

    class ShaderLibrary
    {
    public:
        void Add(const Ref<Shader>& shader);
        void Add(const std::string& name,const Ref<Shader>& shader);

        Ref<Shader> Load(const std::string& path);
        Ref<Shader> Load(const std::string& name, const std::string& path);

        Ref<Shader> Get(const std::string& name);
    private:
        std::unordered_map<std::string, Ref<Shader>> m_shaders;
    };
}
