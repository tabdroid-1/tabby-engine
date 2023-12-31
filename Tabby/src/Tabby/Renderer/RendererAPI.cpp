#include "Tabby/Renderer/RendererAPI.h"
#include "tbpch.h"

#include "Drivers/OpenGL33/OpenGL33RendererAPI.h"

namespace Tabby {

RendererAPI::API RendererAPI::s_API = RendererAPI::API::OpenGL33;

Scope<RendererAPI> RendererAPI::Create()
{
    switch (s_API) {
    case RendererAPI::API::None:
        TB_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
        return nullptr;
    case RendererAPI::API::OpenGL33:
        return CreateScope<OpenGL33RendererAPI>();
    }

    TB_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}

}
