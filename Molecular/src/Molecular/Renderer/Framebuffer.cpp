#include "Framebuffer.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLFramebuffer.h"

namespace Molecular
{
    Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
    {
        switch (Renderer::GetAPI())
        {
            case RendererAPI::API::None      :MOL_CORE_ASSERT(false,"RendererAPI::None not supported"); return nullptr;
			case RendererAPI::API::OpenGL    :return CreateRef<OpenGLFramebuffer>(spec);
        }
        MOL_CORE_ASSERT(false,"Unknown RendererAPI");
        return nullptr;
    }
}
