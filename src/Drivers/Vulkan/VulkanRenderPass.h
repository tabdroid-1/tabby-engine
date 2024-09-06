#pragma once

#include <Drivers/Vulkan/VulkanCommon.h>

namespace Tabby {

class VulkanRenderPass {
public:
    VulkanRenderPass();
    ~VulkanRenderPass();

    void Destroy();

    void Begin(VkCommandBuffer commandBuffer);
    void End(VkCommandBuffer commandBuffer);

    VkRenderPass RawRenderPass() const { return m_RenderPass; }
    std::vector<VkFramebuffer> RawFramebuffer() const { return m_SwapchainFramebuffers; }

private:
    std::vector<VkFramebuffer> m_SwapchainFramebuffers;
    VkRenderPass m_RenderPass;
};

}
