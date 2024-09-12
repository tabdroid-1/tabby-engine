#include <tbpch.h>
#include <Drivers/Vulkan/VulkanShader.h>
#include <Tabby/Renderer/Renderer.h>
#include <Tabby/Renderer/Shader.h>

namespace Tabby {

Shared<Shader> Shader::Create(const ShaderSpecification& spec, std::map<ShaderStage, std::vector<uint32_t>> binaries)
{
    TB_PROFILE_SCOPE_NAME("Tabby::Shader::Create");

    switch (Renderer::GetAPI()) {
    case Renderer::API::Null:
        // return CreateShared<NullAPIShader>(filepath);
        return nullptr;
    case Renderer::API::Vulkan:
        return CreateShared<VulkanShader>(spec, binaries);
    case Renderer::API::OpenGL46:
        // return CreateShared<OpenGL46Shader>(filepath);
        return nullptr;
    case Renderer::API::OpenGL33:
        // return CreateShared<OpenGL33Shader>(filepath);
        return nullptr;
    case Renderer::API::OpenGLES3:
        // return CreateShared<OpenGLES3Shader>(filepath);
        return nullptr;
    }

    TB_CORE_ASSERT_TAGGED(false, "Unknown RendererAPI!");
    return nullptr;
}

}
