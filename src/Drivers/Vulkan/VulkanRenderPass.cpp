#include "VulkanGraphicsContext.h"
#include "VulkanRenderPass.h"
#include "VulkanSwapchain.h"
#include "VulkanDevice.h"
#include "VulkanImage.h"

namespace Tabby {

VulkanRenderPass::VulkanRenderPass()
    : m_Framebuffer(VK_NULL_HANDLE)
    , m_RenderPass(VK_NULL_HANDLE)
{
    m_FramebufferSpecification = {};
}

VulkanRenderPass::~VulkanRenderPass()
{
}

void VulkanRenderPass::CreateRenderPass()
{
    auto device = VulkanGraphicsContext::Get()->GetDevice();
    auto swapchain = VulkanGraphicsContext::Get()->GetSwapchain();

    // std::vector<VkAttachmentReference> color_attachment_references;
    // std::vector<VkAttachmentDescription> attachments;
    // VkAttachmentReference depth_attachment_reference = {};
    //
    // for (int i = 0; i < spec.attachments.size(); i++) {
    //
    //     Shared<VulkanImage> vk_target = ShareAs<VulkanImage>(spec.attachments[i]);
    //     ImageSpecification target_spec = vk_target->GetSpecification();
    //
    //     if (target_spec.usage == ImageUsage::RENDER_TARGET) {
    //         auto& color_attachment = attachments.emplace_back();
    //         color_attachment.format = convert(target_spec.format);
    //         color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    //         color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    //         color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    //         color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    //         color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    //
    //         // color_attachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    //         // if (clear_color.a != 0.0f)
    //         color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    //         // else
    //         //     color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
    //         color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    //
    //         auto& color_attachment_reference = color_attachment_references.emplace_back();
    //         color_attachment_reference.attachment = i;
    //         color_attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    //     } else if (target_spec.usage == ImageUsage::DEPTH_BUFFER) {
    //
    //         auto& depth_attachment = attachments.emplace_back();
    //         depth_attachment.format = VK_FORMAT_D32_SFLOAT_S8_UINT;
    //         depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    //         depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    //         depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    //         depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    //         depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    //         // if (clear_color.a != 0.0f)
    //         depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    //         // else
    //         //     depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
    //
    //         depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    //
    //         depth_attachment_reference.attachment = i;
    //         depth_attachment_reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    //     }
    // }

    VkAttachmentDescription color_attachment {};
    color_attachment.format = VK_FORMAT_R8G8B8A8_UNORM;
    // color_attachment.format = convert(swapchain->GetCurrentImage()->GetSpecification().format);
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentDescription depth_attachment {};
    depth_attachment.format = VK_FORMAT_D32_SFLOAT_S8_UINT;
    depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference color_attachment_reference {};
    color_attachment_reference.attachment = 0;
    color_attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depth_attachment_reference {};
    depth_attachment_reference.attachment = 1;
    depth_attachment_reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_reference;
    subpass.pDepthStencilAttachment = &depth_attachment_reference;

    // VkSubpassDependency dependency {};
    // dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    // dependency.dstSubpass = 0;
    // dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    // dependency.srcAccessMask = 0;
    // dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    // dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    std::array<VkSubpassDependency, 2> dependencies;

    dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[0].dstSubpass = 0;
    dependencies[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    dependencies[0].srcAccessMask = VK_ACCESS_NONE_KHR;
    dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    dependencies[1].srcSubpass = 0;
    dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    std::array<VkAttachmentDescription, 2> attachments = { color_attachment, depth_attachment };
    VkRenderPassCreateInfo renderPassInfo {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = dependencies.size();
    renderPassInfo.pDependencies = dependencies.data();

    VK_CHECK_RESULT(vkCreateRenderPass(VulkanGraphicsContext::Get()->GetDevice()->Raw(), &renderPassInfo, nullptr, &m_RenderPass))
}

void VulkanRenderPass::CreateFramebuffer(const VulkanFramebufferSpecification& spec)
{

    auto device = VulkanGraphicsContext::Get()->GetDevice();
    auto swapchain = VulkanGraphicsContext::Get()->GetSwapchain();

    // check if framebuffer needs to be recreated
    if (m_FramebufferSpecification == spec)
        return;

    // vkDestroyFramebuffer(device->Raw(), m_Framebuffer, nullptr);

    std::vector<VkImageView> attachments = {
        ShareAs<VulkanImage>(spec.color_attachment)->RawView(), ShareAs<VulkanImage>(spec.depth_attachment)->RawView()
    };
    // attachments.resize(spec.attachments.size());
    //
    // for (int i = 0; i < attachments.size(); i++) {
    //     attachments[i] = ShareAs<VulkanImage>(spec.attachments[i])->RawView();
    // }

    VkFramebufferCreateInfo framebufferInfo {};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = m_RenderPass;
    framebufferInfo.attachmentCount = attachments.size();
    framebufferInfo.pAttachments = attachments.data();
    framebufferInfo.width = spec.extent.x;
    framebufferInfo.height = spec.extent.y;
    framebufferInfo.layers = 1;

    VK_CHECK_RESULT(vkCreateFramebuffer(device->Raw(), &framebufferInfo, nullptr, &m_Framebuffer));
}

void VulkanRenderPass::Destroy()
{
    auto device = VulkanGraphicsContext::Get()->GetDevice();

    vkDestroyRenderPass(device->Raw(), m_RenderPass, nullptr);
    vkDestroyFramebuffer(device->Raw(), m_Framebuffer, nullptr);
}
//
// void VulkanRenderPass::DestroyFramebuffer()
// {
//     auto device = VulkanGraphicsContext::Get()->GetDevice();
//
//     // m_DepthStencilBuffer->Destroy();
//
//     // for (auto framebuffer : m_Framebuffers) {
//     //     vkDestroyFramebuffer(device->Raw(), framebuffer, nullptr);
//     // }
//
//     vkDestroyFramebuffer(device->Raw(), m_Framebuffer, nullptr);
// }

// void VulkanRenderPass::Begin(VkCommandBuffer commandBuffer)
// {
//     auto device = VulkanGraphicsContext::Get()->GetDevice();
//     auto swapchain = VulkanGraphicsContext::Get()->GetSwapchain();
//
//     VkRenderPassBeginInfo renderPassInfo {};
//     renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
//     renderPassInfo.renderPass = m_RenderPass;
//     renderPassInfo.framebuffer = m_Framebuffers[swapchain->GetCurrentImageIndex()];
//     renderPassInfo.renderArea.offset = { 0, 0 };
//     renderPassInfo.renderArea.extent = { swapchain->GetSpecification().extent.x, swapchain->GetSpecification().extent.y };
//
//     if (renderPassInfo.renderArea.extent.width != m_WindowSize.width || renderPassInfo.renderArea.extent.height != m_WindowSize.height) {
//         m_WindowSize = { swapchain->GetSpecification().extent.x, swapchain->GetSpecification().extent.y };
//     }
//
//     std::array<VkClearValue, 2> clear_values {};
//     clear_values[0].color = { { 0.0f, 0.0f, 0.0f, 1.0f } };
//     clear_values[1].depthStencil = { 1.0f, 0 };
//
//     renderPassInfo.clearValueCount = clear_values.size();
//     renderPassInfo.pClearValues = clear_values.data();
//
//     vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
//
//     VkViewport viewport {};
//     viewport.x = 0.0f;
//     viewport.y = 0.0f;
//     viewport.width = (float)swapchain->GetSpecification().extent.x;
//     viewport.height = (float)swapchain->GetSpecification().extent.y;
//     viewport.minDepth = 0.0f;
//     viewport.maxDepth = 1.0f;
//     vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
//
//     VkRect2D scissor {};
//     scissor.offset = { 0, 0 };
//     scissor.extent = { swapchain->GetSpecification().extent.x, swapchain->GetSpecification().extent.y };
//     vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
// }

// void VulkanRenderPass::End(VkCommandBuffer commandBuffer)
// {
//
//     vkCmdEndRenderPass(commandBuffer);
// }
}
