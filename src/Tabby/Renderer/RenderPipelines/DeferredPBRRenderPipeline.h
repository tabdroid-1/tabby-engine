#pragma once

#include <Tabby/Renderer/RenderPipeline.h>
#include <Tabby/Renderer/RenderPass.h>
#include <Tabby/Foundation/Types.h>
#include <Tabby/Renderer/Image.h>

namespace Tabby {

class DeferredPBRRenderPipeline : public RenderPipeline {
public:
    DeferredPBRRenderPipeline();
    ~DeferredPBRRenderPipeline();

    void Begin(const Camera& camera) override;
    void End() override;

    Shared<Image> GetFinalImage() override;
    Shared<RenderPass> GetRenderPass() override { return m_RenderPass; }

private:
    Shared<RenderPass> m_RenderPass;

    std::vector<Shared<Image>> m_RendererOutputs;
    std::vector<Shared<Image>> m_DepthAttachments;
    Shared<Image> m_CurrectMainRenderTarget;
    Shared<Image> m_CurrentDepthAttachment;

    std::shared_mutex m_Mutex;
};

}
