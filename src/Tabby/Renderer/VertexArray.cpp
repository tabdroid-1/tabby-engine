#include <tbpch.h>
#include <Drivers/gles3/OpenGLES3VertexArray.h>
#include <Drivers/gl46/OpenGL46VertexArray.h>
#include <Drivers/gl33/OpenGL33VertexArray.h>
#include <Drivers/null/NullAPIVertexArray.h>
#include <Tabby/Renderer/VertexArray.h>
#include <Tabby/Renderer/Renderer.h>

namespace Tabby {

Shared<VertexArray> VertexArray::Create()
{
    TB_PROFILE_SCOPE_NAME("Tabby::VertexArray::Create");

    switch (Renderer::GetAPI()) {
    case RendererAPI::API::Null:
        return CreateShared<NullAPIVertexArray>();
    case RendererAPI::API::OpenGL46:
        return CreateShared<OpenGL46VertexArray>();
    case RendererAPI::API::OpenGL33:
        return CreateShared<OpenGL33VertexArray>();
    case RendererAPI::API::OpenGLES3:
        return CreateShared<OpenGLES3VertexArray>();
    }

    TB_CORE_ASSERT_TAGGED(false, "Unknown RendererAPI!");
    return nullptr;
}

}
