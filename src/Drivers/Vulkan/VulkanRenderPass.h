#pragma once

#include <Drivers/Vulkan/VulkanCommon.h>

#include <Tabby/Renderer/RenderPass.h>

namespace Tabby {

class Image;

class VulkanRenderPass : public RenderPass {
public:
    VulkanRenderPass(const RenderPassSpecification& spec);
    ~VulkanRenderPass();

    void Refresh(const RenderPassSpecification& spec) override;
    void Destroy() override;

    // void Begin() override;
    // void End() override;

    const RenderPassSpecification& GetSpecification() const override { return m_Specification; }

    VkRenderPass Raw() const { return m_RenderPass; }
    VkFramebuffer RawFramebuffer() const;

private:
    RenderPassSpecification m_Specification;

    std::vector<VkFramebuffer> m_Framebuffers;
    VkRenderPass m_RenderPass;
};
}
