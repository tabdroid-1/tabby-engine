#include <tbpch.h>
#include <Drivers/gles3/OpenGLES3Texture.h>
#include <Drivers/gl46/OpenGL46Texture.h>
#include <Drivers/gl33/OpenGL33Texture.h>
#include <Drivers/null/NullAPITexture.h>
#include <Tabby/Renderer/Renderer.h>
#include <Tabby/Renderer/Texture.h>
#include <Tabby/Foundation/Types.h>

namespace Tabby {

Shared<Texture> Texture::Create(const TextureSpecification& specification, AssetHandle handle, Buffer data)
{
    TB_PROFILE_SCOPE_NAME("Tabby::Texture::Create");

    switch (Renderer::GetAPI()) {
    case RendererAPI::API::Null:
        return CreateShared<NullAPITexture>(specification, handle, data);
    case RendererAPI::API::OpenGL46:
        return CreateShared<OpenGL46Texture>(specification, handle, data);
    case RendererAPI::API::OpenGL33:
        return CreateShared<OpenGL33Texture>(specification, handle, data);
    case RendererAPI::API::OpenGLES3:
        return CreateShared<OpenGLES3Texture>(specification, handle, data);
    }

    TB_CORE_ASSERT_TAGGED(false, "Unknown RendererAPI!");
    return nullptr;
}
}
