#include <Drivers/Vulkan/VulkanGraphicsContext.h>
#include <Drivers/Vulkan/VulkanRenderPass.h>
#include <Drivers/Vulkan/VulkanSwapchain.h>
#include <Drivers/Vulkan/VulkanDevice.h>

namespace Tabby {

VulkanRenderPass::VulkanRenderPass()
    : m_RenderPass(VK_NULL_HANDLE)
{
    auto device = VulkanGraphicsContext::Get()->GetDevice();
    auto swapchain = VulkanGraphicsContext::Get()->GetSwapchain();

    VkAttachmentDescription colorAttachment {};
    colorAttachment.format = swapchain->RawSurfaceFormat().format;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkRenderPassCreateInfo renderPassInfo {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;

    VK_CHECK_RESULT(vkCreateRenderPass(device->Raw(), &renderPassInfo, nullptr, &m_RenderPass));

    m_SwapchainFramebuffers.resize(m_SwapchainFramebuffers.size());
    for (size_t i = 0; i < m_SwapchainFramebuffers.size(); i++) {
        VkImageView attachments[] = {
            swapchain->RawImageViews()[i]
        };

        VkFramebufferCreateInfo framebufferInfo {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = m_RenderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = swapchain->GetSpecification().extent.x;
        framebufferInfo.height = swapchain->GetSpecification().extent.y;
        framebufferInfo.layers = 1;

        VK_CHECK_RESULT(vkCreateFramebuffer(device->Raw(), &framebufferInfo, nullptr, &m_SwapchainFramebuffers[i]));
    }
}

VulkanRenderPass::~VulkanRenderPass()
{
}

void VulkanRenderPass::Destroy()
{
    auto device = VulkanGraphicsContext::Get()->GetDevice();

    vkDestroyRenderPass(device->Raw(), m_RenderPass, nullptr);

    for (auto framebuffer : m_SwapchainFramebuffers) {
        vkDestroyFramebuffer(device->Raw(), framebuffer, nullptr);
    }
}

void VulkanRenderPass::Begin(VkCommandBuffer commandBuffer)
{
    auto device = VulkanGraphicsContext::Get()->GetDevice();
    auto swapchain = VulkanGraphicsContext::Get()->GetSwapchain();

    VkRenderPassBeginInfo renderPassInfo {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_RenderPass;
    renderPassInfo.framebuffer = m_SwapchainFramebuffers[0];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = { (uint32_t)swapchain->GetSpecification().extent.x, (uint32_t)swapchain->GetSpecification().extent.y };

    VkClearValue clearColor = { { { 0.0f, 0.0f, 0.0f, 1.0f } } };
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)swapchain->GetSpecification().extent.x;
    viewport.height = (float)swapchain->GetSpecification().extent.y;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor {};
    scissor.offset = { 0, 0 };
    scissor.extent = { (uint32_t)swapchain->GetSpecification().extent.x, (uint32_t)swapchain->GetSpecification().extent.y };
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void VulkanRenderPass::End(VkCommandBuffer commandBuffer)
{
    vkCmdEndRenderPass(commandBuffer);
}
}
