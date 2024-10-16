#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES

#include <Tabby/Renderer/Renderer.h>
#include <Tabby/Renderer/ShaderLibrary.h>

#include <Drivers/Vulkan/VulkanRendererAPI.h>

namespace Tabby {

struct RendererInternalData {
    Shared<ImageSampler> m_LinearSampler;
    Shared<ImageSampler> m_NearestSampler;
} s_InternalData;

void Renderer::Init(const RendererConfig& config)
{
    ShaderLibrary::Init();
    s_RendererAPI = new VulkanRendererAPI(config);

    Tabby::ImageSamplerSpecification sampler_spec = {};
    sampler_spec.min_filtering_mode = Tabby::SamplerFilteringMode::LINEAR;
    sampler_spec.mag_filtering_mode = Tabby::SamplerFilteringMode::LINEAR;
    sampler_spec.mipmap_filtering_mode = Tabby::SamplerFilteringMode::LINEAR;
    sampler_spec.address_mode = Tabby::SamplerAddressMode::REPEAT;
    sampler_spec.min_lod = 0.0f;
    sampler_spec.max_lod = 1000.0f;
    sampler_spec.lod_bias = 0.0f;
    sampler_spec.anisotropic_filtering_level = 16;

    s_InternalData.m_LinearSampler = ImageSampler::Create(sampler_spec);

    sampler_spec.mag_filtering_mode = Tabby::SamplerFilteringMode::NEAREST;
    s_InternalData.m_NearestSampler = ImageSampler::Create(sampler_spec);
}

void Renderer::Shutdown()
{
    ShaderLibrary::Destroy();
    delete s_RendererAPI;
}

RendererConfig Renderer::GetConfig()
{
    return s_RendererAPI->GetConfig();
}

void Renderer::BeginFrame()
{
    s_RendererAPI->BeginFrame();
}

void Renderer::EndFrame()
{
    s_RendererAPI->EndFrame();
}

void Renderer::BeginRender(const std::vector<Shared<Image>> attachments, UIntVector3 render_area, IntVector2 offset, Vector4 clear_value)
{
    s_RendererAPI->BeginRender(attachments, render_area, offset, clear_value);
}

void Renderer::EndRender(Shared<Image> target)
{
    s_RendererAPI->EndRender(target);
}

void Renderer::WaitDevice()
{
    s_RendererAPI->WaitDevice();
}

void Renderer::ClearImage(Shared<Image> image, const Vector4& value)
{
    s_RendererAPI->ClearImage(image, value);
}

void Renderer::RenderTasks(Shared<Mesh> mesh, std::vector<MaterialData> elements, Buffer data)
{
    s_RendererAPI->RenderTasks(mesh, elements, data);
}

void Renderer::RenderTasks(Shared<Shader> shader, uint32_t vertex_count, Buffer data)
{
    s_RendererAPI->RenderTasks(shader, vertex_count, data);
}

uint32_t Renderer::GetCurrentFrameIndex()
{
    return s_RendererAPI->GetCurrentFrameIndex();
}

Shared<ImageSampler> Renderer::GetNearestSampler()
{
    return s_InternalData.m_NearestSampler;
}

Shared<ImageSampler> Renderer::GetLinearSampler()
{
    return s_InternalData.m_LinearSampler;
}

void Renderer::LoadShaderPack()
{
}

void Renderer::Render()
{
    s_RendererAPI->Render();
}

void Renderer::RenderImGui()
{
    s_RendererAPI->RenderImGui();
}

}
