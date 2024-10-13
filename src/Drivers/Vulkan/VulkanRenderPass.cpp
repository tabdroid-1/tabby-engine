#include "VulkanGraphicsContext.h"
#include "VulkanRenderPass.h"
#include "VulkanSwapchain.h"
#include "VulkanDevice.h"
#include "VulkanImage.h"

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
    colorAttachment.format = convert(swapchain->GetCurrentImage()->GetSpecification().format);
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentDescription depthAttachment {};
    depthAttachment.format = VK_FORMAT_D32_SFLOAT_S8_UINT;
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorAttachmentRef {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef {};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    VkSubpassDependency dependency {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
    VkRenderPassCreateInfo renderPassInfo {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(VulkanGraphicsContext::Get()->GetDevice()->Raw(), &renderPassInfo, nullptr, &m_RenderPass) != VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass!");
    }

    m_WindowSize = { swapchain->GetSpecification().extent.x, swapchain->GetSpecification().extent.y };
}

void VulkanRenderPass::CreateFramebuffer()
{
    auto device = VulkanGraphicsContext::Get()->GetDevice();
    auto swapchain = VulkanGraphicsContext::Get()->GetSwapchain();

    if (m_DepthStencilBuffer)
        m_DepthStencilBuffer->Destroy();

    for (auto framebuffer : m_SwapchainFramebuffers) {
        vkDestroyFramebuffer(device->Raw(), framebuffer, nullptr);
    }

    ImageSpecification depthbuffer_spec = ImageSpecification::Default();
    depthbuffer_spec.extent = { swapchain->GetSpecification().extent, 0 };
    depthbuffer_spec.usage = ImageUsage::DEPTH_BUFFER;
    depthbuffer_spec.format = ImageFormat::D32_S8;
    m_DepthStencilBuffer = Image::Create(depthbuffer_spec);

    m_SwapchainFramebuffers.clear();
    m_SwapchainFramebuffers.resize(swapchain->GetImages().size());

    for (size_t i = 0; i < swapchain->GetImages().size(); i++) {
        std::vector<VkImageView> attachments = {
            swapchain->GetImages()[i]->RawView(), ShareAs<VulkanImage>(m_DepthStencilBuffer)->RawView()
        };

        VkFramebufferCreateInfo framebufferInfo {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = m_RenderPass;
        framebufferInfo.attachmentCount = attachments.size();
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = swapchain->GetSpecification().extent.x;
        framebufferInfo.height = swapchain->GetSpecification().extent.y;
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

    m_DepthStencilBuffer->Destroy();

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
    renderPassInfo.renderArea.extent = { swapchain->GetSpecification().extent.x, swapchain->GetSpecification().extent.y };

    if (renderPassInfo.renderArea.extent.width != m_WindowSize.width || renderPassInfo.renderArea.extent.height != m_WindowSize.height) {
        m_WindowSize = { swapchain->GetSpecification().extent.x, swapchain->GetSpecification().extent.y };
    }

    std::array<VkClearValue, 2> clear_values {};
    clear_values[0].color = { { 0.0f, 0.0f, 0.0f, 1.0f } };
    clear_values[1].depthStencil = { 1.0f, 0 };

    renderPassInfo.clearValueCount = clear_values.size();
    renderPassInfo.pClearValues = clear_values.data();

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
    scissor.extent = { swapchain->GetSpecification().extent.x, swapchain->GetSpecification().extent.y };
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void VulkanRenderPass::End(VkCommandBuffer commandBuffer)
{

    vkCmdEndRenderPass(commandBuffer);
}
}
