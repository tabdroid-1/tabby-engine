#include "Tabby/Renderer/Texture.h"
#include "tbpch.h"

#include "Drivers/gl33/OpenGL33Texture.h"
#include "Drivers/gles3/OpenGLES3Texture.h"
#include "Tabby/Renderer/Renderer.h"

namespace Tabby {

Ref<Texture2D> Texture2D::Create(const TextureSpecification& specification)
{
    switch (Renderer::GetAPI()) {
    case RendererAPI::API::None:
        TB_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
        return nullptr;
    case RendererAPI::API::OpenGL33:
        return CreateRef<OpenGL33Texture2D>(specification);
    case RendererAPI::API::OpenGLES3:
        return CreateRef<OpenGLES3Texture2D>(specification);
    }

    TB_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}

Ref<Texture2D> Texture2D::Create(const std::string& path)
{
    switch (Renderer::GetAPI()) {
    case RendererAPI::API::None:
        TB_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
        return nullptr;
    case RendererAPI::API::OpenGL33:
        return CreateRef<OpenGL33Texture2D>(path);
    case RendererAPI::API::OpenGLES3:
        return CreateRef<OpenGLES3Texture2D>(path);
    }

    TB_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}

}
