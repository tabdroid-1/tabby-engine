#include "VulkanGraphicsContext.h"
#include "VulkanRenderPass.h"
#include "VulkanSwapchain.h"
#include "VulkanDevice.h"

namespace Tabby {

VulkanRenderPass::VulkanRenderPass()
    : m_RenderPass(VK_NULL_HANDLE)
{
}

VulkanRenderPass::~VulkanRenderPass()
{
}

void VulkanRenderPass::CreateRenderPass()
{
    auto device = VulkanGraphicsContext::Get()->GetDevice();
    auto swapchain = VulkanGraphicsContext::Get()->GetSwapchain();

    VkAttachmentDescription colorAttachment {};
    colorAttachment.format = swapchain->RawImageFormat();
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

    VkSubpassDependency dependency {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassInfo {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(VulkanGraphicsContext::Get()->GetDevice()->Raw(), &renderPassInfo, nullptr, &m_RenderPass) != VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass!");
    }

    m_WindowSize = { swapchain->RawExtend().width, swapchain->RawExtend().height };
}

void VulkanRenderPass::CreateFramebuffer()
{
    auto device = VulkanGraphicsContext::Get()->GetDevice();
    auto swapchain = VulkanGraphicsContext::Get()->GetSwapchain();

    for (auto framebuffer : m_SwapchainFramebuffers) {
        vkDestroyFramebuffer(device->Raw(), framebuffer, nullptr);
    }

    m_SwapchainFramebuffers.clear();
    m_SwapchainFramebuffers.resize(swapchain->RawImageViews().size());

    for (size_t i = 0; i < swapchain->RawImageViews().size(); i++) {
        VkImageView attachments[] = { swapchain->RawImageViews()[i] };

        VkFramebufferCreateInfo framebufferInfo {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = m_RenderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = swapchain->RawExtend().width;
        framebufferInfo.height = swapchain->RawExtend().height;
        framebufferInfo.layers = 1;

        VK_CHECK_RESULT(vkCreateFramebuffer(device->Raw(), &framebufferInfo, nullptr, &m_SwapchainFramebuffers[i]));
    }
}

void VulkanRenderPass::DestroyRenderPass()
{
    auto device = VulkanGraphicsContext::Get()->GetDevice();

    vkDestroyRenderPass(device->Raw(), m_RenderPass, nullptr);
}

void VulkanRenderPass::DestroyFramebuffer()
{
    auto device = VulkanGraphicsContext::Get()->GetDevice();

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
    renderPassInfo.framebuffer = m_SwapchainFramebuffers[swapchain->GetCurrentImageIndex()];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = { (uint32_t)swapchain->RawExtend().width, (uint32_t)swapchain->RawExtend().height };

    if (renderPassInfo.renderArea.extent.width != m_WindowSize.width || renderPassInfo.renderArea.extent.height != m_WindowSize.height) {
        //
        //     for (auto framebuffer : m_SwapchainFramebuffers) {
        //         vkDestroyFramebuffer(device->Raw(), framebuffer, nullptr);
        //     }
        //
        //     m_SwapchainFramebuffers.clear();
        //     m_SwapchainFramebuffers.resize(swapchain->RawImageViews().size());
        //     for (size_t i = 0; i < swapchain->RawImageViews().size(); i++) {
        //         VkImageView attachments[] = { swapchain->RawImageViews()[i] };
        //
        //         VkFramebufferCreateInfo framebufferInfo {};
        //         framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        //         framebufferInfo.renderPass = m_RenderPass;
        //         framebufferInfo.attachmentCount = 1;
        //         framebufferInfo.pAttachments = attachments;
        //         framebufferInfo.width = swapchain->RawExtend().width;
        //         framebufferInfo.height = swapchain->RawExtend().height;
        //         framebufferInfo.layers = 1;
        //
        //         VK_CHECK_RESULT(vkCreateFramebuffer(device->Raw(), &framebufferInfo, nullptr, &m_SwapchainFramebuffers[i]));
        //     }
        //
        m_WindowSize = { swapchain->RawExtend().width, swapchain->RawExtend().height };
    }

    VkClearValue clearColor = { { { 0.0f, 0.0f, 0.0f, 1.0f } } };
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)swapchain->RawExtend().width;
    viewport.height = (float)swapchain->RawExtend().height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor {};
    scissor.offset = { 0, 0 };
    scissor.extent = { (uint32_t)swapchain->RawExtend().width, (uint32_t)swapchain->RawExtend().height };
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void VulkanRenderPass::End(VkCommandBuffer commandBuffer)
{

    vkCmdEndRenderPass(commandBuffer);
}
}
