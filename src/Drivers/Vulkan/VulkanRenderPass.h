#pragma once

#include "VulkanCommon.h"

namespace Tabby {

class VulkanRenderPass {
public:
    VulkanRenderPass();
    ~VulkanRenderPass();

    void CreateRenderPass();
    void CreateFramebuffer();
    void DestroyRenderPass();
    void DestroyFramebuffer();

    void Begin(VkCommandBuffer commandBuffer);
    void End(VkCommandBuffer commandBuffer);

    VkRenderPass Raw() const { return m_RenderPass; }
    std::vector<VkFramebuffer> RawFramebuffer() const { return m_SwapchainFramebuffers; }

private:
    std::vector<VkFramebuffer> m_SwapchainFramebuffers;
    VkRenderPass m_RenderPass;

    VkExtent2D m_WindowSize;
};
}
