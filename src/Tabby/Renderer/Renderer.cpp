#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES

#include <Tabby/Renderer/Renderer.h>
#include <Tabby/Renderer/ShaderLibrary.h>
#include <Tabby/Renderer/RenderPipelines/DeferredPBRRenderPipeline.h>

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

    // TODO:
    // s_RenderPipeline = config.render_pipeline;
    s_RenderPipeline = new DeferredPBRRenderPipeline();

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

void Renderer::Submit(RenderFunction func)
{
    s_RendererAPI->Submit(func);
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

void Renderer::BeginRender(Shared<RenderPass> render_pass, const std::vector<Shared<Image>>& attachments, UIntVector3 render_area, IntVector2 offset, Vector4 clear_value)
{
    s_RendererAPI->BeginRender(render_pass, attachments, render_area, offset, clear_value);
}

void Renderer::EndRender(Shared<Image> target)
{
    s_RendererAPI->EndRender(target);
}

void Renderer::BeginRenderPipeline(const Camera& camera)
{
    s_RenderPipeline->Begin(camera);
}

void Renderer::EndRenderPipeline()
{
    s_RenderPipeline->End();
}

void Renderer::WaitDevice()
{
    s_RendererAPI->WaitDevice();
}

void Renderer::CopyToSwapchain(Shared<Image> image)
{
    s_RendererAPI->CopyToSwapchain(image);
}

void Renderer::ClearImage(Shared<Image> image, const Vector4& value)
{
    s_RendererAPI->ClearImage(image, value);
}

void Renderer::RenderTasks(Shared<Mesh> mesh, std::vector<MaterialData> elements, Buffer data)
{
    s_RendererAPI->RenderTasks(mesh, elements, data);
}

void Renderer::RenderTasks(Shared<Pipeline> pipeline, uint32_t vertex_count, Buffer data)
{
    s_RendererAPI->RenderTasks(pipeline, vertex_count, data);
}

void Renderer::RenderQuad(Shared<Pipeline> pipeline, Buffer data)
{
    s_RendererAPI->RenderQuad(pipeline, data);
}

void Renderer::RenderQuad(Shared<Pipeline> pipeline, uint32_t amount, Buffer data)
{
    s_RendererAPI->RenderQuad(pipeline, amount, data);
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
