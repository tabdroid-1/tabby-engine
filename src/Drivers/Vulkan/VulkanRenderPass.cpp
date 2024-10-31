#include "VulkanGraphicsContext.h"
#include "VulkanRenderPass.h"
#include "VulkanSwapchain.h"
#include "VulkanDevice.h"
#include "VulkanImage.h"

namespace Tabby {

VulkanRenderPass::VulkanRenderPass(const RenderPassSpecification& spec)
    : m_Framebuffers({ VK_NULL_HANDLE, VK_NULL_HANDLE })
    , m_RenderPass(VK_NULL_HANDLE)
{
    Refresh(spec);
}

VulkanRenderPass::~VulkanRenderPass()
{
}

void VulkanRenderPass::Refresh(const RenderPassSpecification& spec)
{
    auto device = VulkanGraphicsContext::Get()->GetDevice();
    auto swapchain = VulkanGraphicsContext::Get()->GetSwapchain();

    // Create render pass
    if (spec.attachments != m_Specification.attachments || spec.clear_color != m_Specification.clear_color) {

        bool has_depth = false;
        std::vector<VkAttachmentReference> color_attachment_references;
        std::vector<VkAttachmentDescription> attachments;
        VkAttachmentReference depth_attachment_reference = {};

        for (int i = 0; i < spec.attachments.size(); i++) {

            Shared<VulkanImage> vk_target = ShareAs<VulkanImage>(spec.attachments[i]);
            ImageSpecification target_spec = vk_target->GetSpecification();

            if (target_spec.usage == ImageUsage::RENDER_TARGET) {
                auto& color_attachment = attachments.emplace_back();
                color_attachment.format = convert(target_spec.format);
                color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
                color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                color_attachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

                if (spec.clear_color.a != 0.0f)
                    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                else
                    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
                color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

                auto& color_attachment_reference = color_attachment_references.emplace_back();
                color_attachment_reference.attachment = i;
                color_attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            } else if (target_spec.usage == ImageUsage::DEPTH_BUFFER) {

                auto& depth_attachment = attachments.emplace_back();
                depth_attachment.format = VK_FORMAT_D32_SFLOAT_S8_UINT;
                depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
                depth_attachment.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
                depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
                depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

                depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
                // depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
                //
                // depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;

                if (spec.clear_color.a != 0.0f) {
                    depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                    depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;

                } else {
                    depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
                    depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
                }

                depth_attachment_reference.attachment = i;
                depth_attachment_reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

                has_depth = true;
            }
        }

        VkSubpassDescription subpass {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = color_attachment_references.size();
        subpass.pColorAttachments = color_attachment_references.data();
        if (has_depth)
            subpass.pDepthStencilAttachment = &depth_attachment_reference;

        // VkSubpassDependency dependency {};
        // dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        // dependency.dstSubpass = 0;
        // dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        // dependency.srcAccessMask = 0;
        // dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        // dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        // if (has_depth) {
        //     dependency.dstAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        //     dependency.srcStageMask |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        //     dependency.dstStageMask |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        // }
        const VkPipelineStageFlags graphicsStages = 0
            | VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT
            | VK_PIPELINE_STAGE_VERTEX_INPUT_BIT
            | VK_PIPELINE_STAGE_VERTEX_SHADER_BIT
            | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
            | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT
            | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT
            | VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        const VkPipelineStageFlags outsideStages = 0
            | graphicsStages
            | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT
            | VK_PIPELINE_STAGE_TRANSFER_BIT;

        VkSubpassDependency subpass_dependency;

        subpass_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        subpass_dependency.dstSubpass = 0;
        subpass_dependency.srcStageMask = outsideStages;
        subpass_dependency.dstStageMask = graphicsStages;
        subpass_dependency.srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT;
        subpass_dependency.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        subpass_dependency.dependencyFlags = 0;

        // dep[1].srcSubpass = 0;
        // dep[1].dstSubpass = VK_SUBPASS_EXTERNAL;
        // dep[1].srcStageMask = graphicsStages;
        // dep[1].dstStageMask = outsideStages;
        // dep[1].srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT;
        // dep[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT;
        // dep[1].dependencyFlags = 0;

        VkRenderPassCreateInfo renderPassInfo {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &subpass_dependency;

        VK_CHECK_RESULT(vkCreateRenderPass(VulkanGraphicsContext::Get()->GetDevice()->Raw(), &renderPassInfo, nullptr, &m_RenderPass))
    }

    // create framebuffer
    {

        auto device = VulkanGraphicsContext::Get()->GetDevice();
        auto swapchain = VulkanGraphicsContext::Get()->GetSwapchain();

        vkDestroyFramebuffer(device->Raw(), m_Framebuffers[swapchain->GetCurrentFrameIndex()], nullptr);

        std::vector<VkImageView> attachments;
        attachments.resize(spec.attachments.size());

        for (int i = 0; i < attachments.size(); i++) {
            attachments[i] = ShareAs<VulkanImage>(spec.attachments[i])->RawView();
        }

        VkFramebufferCreateInfo framebufferInfo {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = m_RenderPass;
        framebufferInfo.attachmentCount = attachments.size();
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = spec.extent.x;
        framebufferInfo.height = spec.extent.y;
        framebufferInfo.layers = 1;

        VK_CHECK_RESULT(vkCreateFramebuffer(device->Raw(), &framebufferInfo, nullptr, &m_Framebuffers[swapchain->GetCurrentFrameIndex()]));
    }

    m_Specification = spec;
}

void VulkanRenderPass::Destroy()
{
    auto device = VulkanGraphicsContext::Get()->GetDevice();

    vkDestroyRenderPass(device->Raw(), m_RenderPass, nullptr);

    for (auto framebuffer : m_Framebuffers)
        vkDestroyFramebuffer(device->Raw(), framebuffer, nullptr);
}

VkFramebuffer VulkanRenderPass::RawFramebuffer() const
{
    auto swapchain = VulkanGraphicsContext::Get()->GetSwapchain();

    return m_Framebuffers[swapchain->GetCurrentFrameIndex()];
}
}
