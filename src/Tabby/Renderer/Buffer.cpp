#include <tbpch.h>
#include <Drivers/gles3/OpenGLES3Buffer.h>
#include <Drivers/gl46/OpenGL46Buffer.h>
#include <Drivers/gl33/OpenGL33Buffer.h>
#include <Drivers/null/NullAPIBuffer.h>
#include <Tabby/Renderer/Renderer.h>
#include <Tabby/Renderer/Buffer.h>

namespace Tabby {

Shared<VertexBuffer> VertexBuffer::Create(uint32_t size)
{
    TB_PROFILE_SCOPE_NAME("Tabby::VertexBuffer::Create");

    switch (Renderer::GetAPI()) {
    case RendererAPI::API::Null:
        return CreateShared<NullAPIVertexBuffer>(size);
    case RendererAPI::API::OpenGL46:
        return CreateShared<OpenGL46VertexBuffer>(size);
    case RendererAPI::API::OpenGL33:
        return CreateShared<OpenGL33VertexBuffer>(size);
    case RendererAPI::API::OpenGLES3:
        return CreateShared<OpenGLES3VertexBuffer>(size);
    }

    TB_CORE_ASSERT_TAGGED(false, "Unknown RendererAPI!");
    return nullptr;
}

Shared<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size)
{
    TB_PROFILE_SCOPE_NAME("Tabby::VertexBuffer::Create");

    switch (Renderer::GetAPI()) {
    case RendererAPI::API::Null:
        return CreateShared<NullAPIVertexBuffer>(vertices, size);
    case RendererAPI::API::OpenGL46:
        return CreateShared<OpenGL46VertexBuffer>(vertices, size);
    case RendererAPI::API::OpenGL33:
        return CreateShared<OpenGL33VertexBuffer>(vertices, size);
    case RendererAPI::API::OpenGLES3:
        return CreateShared<OpenGLES3VertexBuffer>(vertices, size);
    }

    TB_CORE_ASSERT_TAGGED(false, "Unknown RendererAPI!");
    return nullptr;
}

Shared<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t size)
{
    TB_PROFILE_SCOPE_NAME("Tabby::IndexBuffer::Create");

    switch (Renderer::GetAPI()) {
    case RendererAPI::API::Null:
        return CreateShared<NullAPIIndexBuffer>(indices, size);
    case RendererAPI::API::OpenGL46:
        return CreateShared<OpenGL46IndexBuffer>(indices, size);
    case RendererAPI::API::OpenGL33:
        return CreateShared<OpenGL33IndexBuffer>(indices, size);
    case RendererAPI::API::OpenGLES3:
        return CreateShared<OpenGLES3IndexBuffer>(indices, size);
    }

    TB_CORE_ASSERT_TAGGED(false, "Unknown RendererAPI!");
    return nullptr;
}

}
