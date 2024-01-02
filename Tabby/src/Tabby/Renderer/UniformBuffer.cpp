#include "UniformBuffer.h"
#include "tbpch.h"

#include "Drivers/gl33/OpenGL33UniformBuffer.h"
#include "Tabby/Renderer/Renderer.h"

namespace Tabby {

Ref<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding)
{
    switch (Renderer::GetAPI()) {
    case RendererAPI::API::None:
        TB_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
        return nullptr;
    case RendererAPI::API::OpenGL33:
        return CreateRef<OpenGL33UniformBuffer>(size, binding);
    }

    TB_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}

}
