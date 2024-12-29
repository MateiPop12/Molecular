#include "Renderer/Texture.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLTexture.h"

namespace Molecular
{
    Ref<Texture2D> Texture2D::Create(const std::string& path)
    {
        switch (Renderer::GetAPI())
        {
            case RendererAPI::API::None      :MOL_CORE_ASSERT(false,"RendererAPI::None not supported"); return nullptr;
            case RendererAPI::API::OpenGL    :return std::make_shared<OpenGLTexture2D>(path);
        }
        MOL_CORE_ASSERT(false,"Unknown RendererAPI");
        return nullptr;
    }
}
