#pragma once

#include <Tabby/Renderer/GraphicsContext.h>
#include <Drivers/Vulkan/VulkanCommon.h>
#include <Tabby/Renderer/Renderer.h>

namespace Tabby {

class VulkanDevice;
class VulkanShader;
class VulkanSwapchain;
class VulkanDebugUtils;

class VulkanGraphicsContext : public GraphicsContext {
public:
    VulkanGraphicsContext() {};
    VulkanGraphicsContext(const RendererConfig& config);
    ~VulkanGraphicsContext();
    void Destroy() override;

    static VulkanGraphicsContext* Get() { return s_Instance; }

    VkInstance GetVulkanInstance() const { return m_VulkanInstance; }
    Shared<VulkanDevice> GetDevice() const { return m_Device; }
    Shared<VulkanSwapchain> GetSwapchain() { return m_Swapchain; }

private:
    static std::vector<const char*> GetVulkanExtensions();
    static std::vector<const char*> GetVulkanLayers();

private:
    inline static VulkanGraphicsContext* s_Instance;

    VkInstance m_VulkanInstance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT m_DebugMessenger;
    Shared<VulkanDevice> m_Device = nullptr;
    Shared<VulkanDebugUtils> m_DebugUtils = nullptr;
    Shared<VulkanSwapchain> m_Swapchain = nullptr;
    Shared<VulkanShader> m_Shader = nullptr;
};

}
