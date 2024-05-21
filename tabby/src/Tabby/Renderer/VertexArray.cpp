#include "tbpch.h"

#include "Drivers/gl33/OpenGL33VertexArray.h"
#include "Drivers/gles3/OpenGLES3VertexArray.h"
#include "Tabby/Renderer/Renderer.h"
#include "Tabby/Renderer/VertexArray.h"

namespace Tabby {

Shared<VertexArray> VertexArray::Create()
{
    switch (Renderer::GetAPI()) {
    case RendererAPI::API::None:
        TB_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
        return nullptr;
    case RendererAPI::API::OpenGL33:
        return CreateShared<OpenGL33VertexArray>();
    case RendererAPI::API::OpenGLES3:
        return CreateShared<OpenGLES3VertexArray>();
    }

    TB_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}

}
