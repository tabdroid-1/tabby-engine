// #pragma once
//
// #include <Drivers/Vulkan/VulkanCommon.h>
//
// namespace Tabby {
//
// class Image;
//
// struct VulkanFramebufferSpecification {
//     // std::vector<Shared<Image>> attachments;
//     Shared<Image> color_attachment;
//     Shared<Image> depth_attachment;
//     UIntVector2 extent;
//
//     bool operator==(const VulkanFramebufferSpecification& a) const
//     {
//         return (this->depth_attachment == a.depth_attachment && this->color_attachment == a.color_attachment && this->extent == a.extent) ? true : false;
//     }
// };
//
// class VulkanRenderPass {
// public:
//     VulkanRenderPass();
//     ~VulkanRenderPass();
//
//     void CreateRenderPass();
//     void CreateFramebuffer(const VulkanFramebufferSpecification& spec);
//     void Destroy();
//
//     VkRenderPass Raw() const { return m_RenderPass; }
//     VkFramebuffer RawFramebuffer() const { return m_Framebuffer; }
//     // std::vector<VkFramebuffer> RawFramebuffer() const { return m_Framebuffers; }
//
// private:
//     VulkanFramebufferSpecification m_FramebufferSpecification;
//     VkFramebuffer m_Framebuffer;
//     // std::vector<VkFramebuffer> m_Framebuffers;
//     // Shared<Image> m_DepthStencilBuffer;
//     VkRenderPass m_RenderPass;
// };
// }
