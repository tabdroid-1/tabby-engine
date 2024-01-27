#include "Tabby/Renderer/RendererAPI.h"
#include "tbpch.h"

#include "Drivers/gl33/OpenGL33RendererAPI.h"
#include "Drivers/gles3/OpenGLES3RendererAPI.h"

namespace Tabby {

#ifdef TB_PLATFORM_WEB || TB_PLATFORM_ANDROID
RendererAPI::API RendererAPI::s_API = RendererAPI::API::OpenGLES3;
#else
RendererAPI::API RendererAPI::s_API = RendererAPI::API::OpenGL33;
#endif

Scope<RendererAPI> RendererAPI::Create()
{
    switch (s_API) {
    case RendererAPI::API::None:
        TB_CORE_ASSERT(false, "No renderer API selected.");
        return nullptr;
    case RendererAPI::API::OpenGL33:
        return CreateScope<OpenGL33RendererAPI>();
    case RendererAPI::API::OpenGLES3:
        return CreateScope<OpenGLES3RendererAPI>();
    }

    TB_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}

}
