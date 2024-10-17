#include "VulkanCommon.h"

#include <Tabby/Renderer/GraphicsContext.h>
#include <Tabby/Renderer/Renderer.h>

namespace Tabby {

class VulkanDeviceCmdBuffer;
class VulkanDebugUtils;
class VulkanSwapchain;
class VulkanDevice;
class VulkanShader;

class VulkanGraphicsContext : public GraphicsContext {
public:
    VulkanGraphicsContext() {};
    VulkanGraphicsContext(const RendererConfig& config);
    ~VulkanGraphicsContext();
    void Destroy() override;

    static VulkanGraphicsContext* Get() { return s_Instance; }

    VkInstance GetVulkanInstance() const { return m_VulkanInstance; }
    std::shared_ptr<VulkanDevice> GetDevice() const { return m_Device; }
    std::shared_ptr<VulkanSwapchain> GetSwapchain() { return m_Swapchain; }
    // std::shared_ptr<VulkanRenderPass> GetRenderPass() { return m_RenderPass; }

    static constexpr int GetFramesInFlight() { return 2; };

    inline static PFN_vkCmdPipelineBarrier2KHR vkCmdPipelineBarrier2KHR;

private:
    static std::vector<const char*> GetVulkanExtensions();
    static std::vector<const char*> GetVulkanLayers();

private:
    inline static VulkanGraphicsContext* s_Instance;

    VkInstance m_VulkanInstance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT m_DebugMessenger;
    std::shared_ptr<VulkanDevice> m_Device = nullptr;
    std::shared_ptr<VulkanDebugUtils> m_DebugUtils = nullptr;
    std::shared_ptr<VulkanSwapchain> m_Swapchain = nullptr;
    // std::shared_ptr<VulkanRenderPass> m_RenderPass = nullptr;
};

}
