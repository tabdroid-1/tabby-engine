#pragma once

#include <Drivers/Vulkan/VulkanCommon.h>

namespace Tabby {

class Image;

struct VulkanRenderPassSpecification {
    std::vector<Shared<Image>> attachments;
    Vector4 clear_color;
    UIntVector3 extent;

    bool operator==(const VulkanRenderPassSpecification& a) const
    {
        return (this->attachments == a.attachments && this->extent == a.extent && this->clear_color == a.clear_color) ? true : false;
    }
};

class VulkanRenderPass {
public:
    VulkanRenderPass();
    ~VulkanRenderPass();

    void Create(const VulkanRenderPassSpecification& spec);
    void Destroy();

    VkRenderPass Raw() const { return m_RenderPass; }
    VkFramebuffer RawFramebuffer() const { return m_Framebuffer; }
    // std::vector<VkFramebuffer> RawFramebuffer() const { return m_Framebuffers; }

    // private:
    //     void CreateFramebuffer(const VulkanRenderPassSpecification& spec);

private:
    VulkanRenderPassSpecification m_Specification;
    VkFramebuffer m_Framebuffer;
    // std::vector<VkFramebuffer> m_Framebuffers;
    // Shared<Image> m_DepthStencilBuffer;
    VkRenderPass m_RenderPass;
};
}
