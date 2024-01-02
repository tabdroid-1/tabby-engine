#include "Tabby/Renderer/Buffer.h"
#include "tbpch.h"

#include "Tabby/Renderer/Renderer.h"

#include "Drivers/gl33/OpenGL33Buffer.h"
#include "Drivers/gles3/OpenGLES3Buffer.h"

namespace Tabby {

Ref<VertexBuffer> VertexBuffer::Create(uint32_t size)
{
    switch (Renderer::GetAPI()) {
    case RendererAPI::API::None:
        TB_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
        return nullptr;
    case RendererAPI::API::OpenGL33:
        return CreateRef<OpenGL33VertexBuffer>(size);
    case RendererAPI::API::OpenGLES3:
        return CreateRef<OpenGLES3VertexBuffer>(size);
    }

    TB_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}

Ref<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size)
{
    switch (Renderer::GetAPI()) {
    case RendererAPI::API::None:
        TB_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
        return nullptr;
    case RendererAPI::API::OpenGL33:
        return CreateRef<OpenGL33VertexBuffer>(vertices, size);
    case RendererAPI::API::OpenGLES3:
        return CreateRef<OpenGLES3VertexBuffer>(vertices, size);
    }

    TB_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}

Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t size)
{
    switch (Renderer::GetAPI()) {
    case RendererAPI::API::None:
        TB_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
        return nullptr;
    case RendererAPI::API::OpenGL33:
        return CreateRef<OpenGL33IndexBuffer>(indices, size);
    case RendererAPI::API::OpenGLES3:
        return CreateRef<OpenGLES3IndexBuffer>(indices, size);
    }

    TB_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}

}
