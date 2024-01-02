#include "tbpch.h"

#include "Drivers/gl33/OpenGL33VertexArray.h"
#include "Tabby/Renderer/Renderer.h"
#include "Tabby/Renderer/VertexArray.h"

namespace Tabby {

Ref<VertexArray> VertexArray::Create()
{
    switch (Renderer::GetAPI()) {
    case RendererAPI::API::None:
        TB_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
        return nullptr;
    case RendererAPI::API::OpenGL33:
        return CreateRef<OpenGL33VertexArray>();
    }

    TB_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}

}
