#include "Tabby/Renderer/Framebuffer.h"
#include "tbpch.h"

#include "Tabby/Renderer/Renderer.h"

#include "Drivers/gl33/OpenGL33Framebuffer.h"

namespace Tabby {

Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
{
    switch (Renderer::GetAPI()) {
    case RendererAPI::API::None:
        TB_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
        return nullptr;
    case RendererAPI::API::OpenGL33:
        return CreateRef<OpenGL33Framebuffer>(spec);
    }

    TB_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}

}
