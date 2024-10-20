#include <tbpch.h>
#include <Drivers/gles3/OpenGLES3RendererAPI.h>
#include <Drivers/gl46/OpenGL46RendererAPI.h>
#include <Drivers/gl33/OpenGL33RendererAPI.h>
#include <Drivers/null/NullAPIRendererAPI.h>
#include <Tabby/Renderer/RendererAPI.h>

namespace Tabby {

RendererAPI::API RendererAPI::s_API = RendererAPI::API::OpenGL33;

Scope<RendererAPI> RendererAPI::Create()
{
    TB_PROFILE_SCOPE_NAME("Tabby::RendererAPI::Create");

    switch (s_API) {
    case RendererAPI::API::Null:
        return CreateScope<NullAPIRendererAPI>();
    case RendererAPI::API::OpenGL46:
        return CreateScope<OpenGL46RendererAPI>();
    case RendererAPI::API::OpenGL33:
        return CreateScope<OpenGL33RendererAPI>();
    case RendererAPI::API::OpenGLES3:
        return CreateScope<OpenGLES3RendererAPI>();
    }

    TB_CORE_ASSERT_TAGGED(false, "Unknown RendererAPI!");
    return nullptr;
}

}
