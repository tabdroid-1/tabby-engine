#include "Tabby/Renderer/GraphicsContext.h"
#include "tbpch.h"

#include "Drivers/OpenGL33/OpenGL33Context.h"
#include "Tabby/Renderer/Renderer.h"

namespace Tabby {

Scope<GraphicsContext> GraphicsContext::Create(void* window)
{
    switch (Renderer::GetAPI()) {
    case RendererAPI::API::None:
        TB_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
        return nullptr;
    case RendererAPI::API::OpenGL33:
        return CreateScope<OpenGL33Context>(static_cast<GLFWwindow*>(window));
    }

    TB_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}

}
