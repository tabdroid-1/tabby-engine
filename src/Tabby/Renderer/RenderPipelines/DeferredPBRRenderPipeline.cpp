#include <Tabby/Renderer/RenderPipelines/DeferredPBRRenderPipeline.h>
#include <Tabby/Renderer/Renderer.h>
#include <Tabby/Core/Application.h>

namespace Tabby {

DeferredPBRRenderPipeline::DeferredPBRRenderPipeline()
{

    // Initialize render targets
    ImageSpecification attachment_spec = ImageSpecification::Default();
    attachment_spec.usage = ImageUsage::RENDER_TARGET;
    attachment_spec.extent = { Application::GetWindow().GetWidth(), Application::GetWindow().GetHeight(), 1 };
    attachment_spec.format = ImageFormat::RGBA32_UNORM;
    attachment_spec.path = "bin_world_renderer_render_target";
    for (int i = 0; i < 2; i++)
        m_RendererOutputs.push_back(Image::Create(attachment_spec));

    attachment_spec.usage = ImageUsage::DEPTH_BUFFER;
    attachment_spec.format = ImageFormat::D32_S8;
    attachment_spec.path = "bin_world_renderer_depth_buffer";

    for (int i = 0; i < 2; i++)
        m_DepthAttachments.push_back(Image::Create(attachment_spec));

    m_CurrectMainRenderTarget = m_RendererOutputs[Renderer::GetCurrentFrameIndex()];
    m_CurrentDepthAttachment = m_DepthAttachments[Renderer::GetCurrentFrameIndex()];

    RenderPassSpecification render_pass_spec;
    render_pass_spec.extent = { Application::GetWindow().GetWidth(), Application::GetWindow().GetHeight(), 1 };
    render_pass_spec.clear_color = { 0.0f, 0.0f, 0.0f, 1.0f };
    render_pass_spec.attachments = { m_CurrectMainRenderTarget, m_CurrentDepthAttachment };

    m_RenderPass = RenderPass::Create(render_pass_spec);
}

DeferredPBRRenderPipeline::~DeferredPBRRenderPipeline()
{
    m_RenderPass->Destroy();

    for (int i = 0; i < 2; i++)
        m_RendererOutputs[i]->Destroy();

    for (int i = 0; i < 2; i++)
        m_DepthAttachments[i]->Destroy();
}

void DeferredPBRRenderPipeline::Begin(const Camera& camera)
{

    Renderer::Submit([=]() {
        m_CurrectMainRenderTarget = m_RendererOutputs[Renderer::GetCurrentFrameIndex()];
        m_CurrentDepthAttachment = m_DepthAttachments[Renderer::GetCurrentFrameIndex()];

        if (m_RendererOutputs[Renderer::GetCurrentFrameIndex()]->GetSpecification().extent != UIntVector3 { Application::GetWindow().GetWidth(), Application::GetWindow().GetHeight(), 1 }) {
            ImageSpecification attachment_spec = ImageSpecification::Default();
            attachment_spec.usage = ImageUsage::RENDER_TARGET;
            attachment_spec.extent = { Application::GetWindow().GetWidth(), Application::GetWindow().GetHeight(), 1 };
            attachment_spec.format = ImageFormat::RGBA32_UNORM;
            attachment_spec.path = "bin_deferred_pbr_renderer_render_target";

            m_RendererOutputs[Renderer::GetCurrentFrameIndex()]->Destroy();
            m_RendererOutputs[Renderer::GetCurrentFrameIndex()] = Image::Create(attachment_spec);
            m_CurrectMainRenderTarget = m_RendererOutputs[Renderer::GetCurrentFrameIndex()];

            attachment_spec.usage = ImageUsage::DEPTH_BUFFER;
            attachment_spec.format = ImageFormat::D32_S8;
            attachment_spec.path = "bin_deferred_pbr_renderer_depth_buffer";

            m_DepthAttachments[Renderer::GetCurrentFrameIndex()]->Destroy();
            m_DepthAttachments[Renderer::GetCurrentFrameIndex()] = Image::Create(attachment_spec);
            m_CurrentDepthAttachment = m_DepthAttachments[Renderer::GetCurrentFrameIndex()];
        }
    });

    Renderer::BeginRender(m_RenderPass, { m_CurrectMainRenderTarget, m_CurrentDepthAttachment }, m_CurrectMainRenderTarget->GetSpecification().extent, { 0, 0 }, { 0.2f, 0.2f, 0.2f, 1.0f });
}

void DeferredPBRRenderPipeline::End()
{
    Renderer::EndRender({ m_CurrectMainRenderTarget });
}

Shared<Image> DeferredPBRRenderPipeline::GetFinalImage()
{
    return m_CurrectMainRenderTarget;
}

}
