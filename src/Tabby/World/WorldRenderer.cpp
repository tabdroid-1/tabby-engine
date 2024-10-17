#include <Tabby/World/WorldRenderer.h>
#include <Tabby/Renderer/Renderer.h>
#include <Tabby/Core/Application.h>

namespace Tabby {

Shared<WorldRenderer> WorldRenderer::Create()
{
    return CreateShared<WorldRenderer>();
}

WorldRenderer::WorldRenderer()
{
    // Initialize render targets
    ImageSpecification attachment_spec = ImageSpecification::Default();
    attachment_spec.usage = ImageUsage::RENDER_TARGET;
    attachment_spec.extent = { Application::GetWindow().GetWidth(), Application::GetWindow().GetHeight(), 1 };
    attachment_spec.format = ImageFormat::RGBA32_UNORM;
    for (int i = 0; i < 2; i++)
        m_RendererOutputs.push_back(Image::Create(attachment_spec));

    attachment_spec.usage = ImageUsage::DEPTH_BUFFER;
    attachment_spec.format = ImageFormat::D32_S8;

    for (int i = 0; i < 2; i++)
        m_DepthAttachments.push_back(Image::Create(attachment_spec));
}

WorldRenderer::~WorldRenderer()
{
}

void WorldRenderer::Destroy()
{
    for (int i = 0; i < 2; i++)
        m_RendererOutputs[i]->Destroy();

    for (int i = 0; i < 2; i++)
        m_DepthAttachments[i]->Destroy();
}

void WorldRenderer::BeginScene()
{
    m_CurrectMainRenderTarget = m_RendererOutputs[Renderer::GetCurrentFrameIndex()];
    m_CurrentDepthAttachment = m_DepthAttachments[Renderer::GetCurrentFrameIndex()];
    Renderer::BeginRender({ m_CurrectMainRenderTarget, m_CurrentDepthAttachment }, m_CurrectMainRenderTarget->GetSpecification().extent, { 0, 0 }, { 0.2f, 0.2f, 0.2f, 1.0f });
}

void WorldRenderer::EndScene()
{
    Renderer::EndRender({ m_CurrectMainRenderTarget });
}

Shared<Image> WorldRenderer::GetFinalImage()
{
    return m_CurrectMainRenderTarget;
}

}
