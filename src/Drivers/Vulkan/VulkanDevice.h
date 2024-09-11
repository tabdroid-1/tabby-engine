#pragma once

#include "VulkanCommon.h"

namespace Tabby {
class VulkanGraphicsContext;

struct QueueFamilyIndex {
    std::optional<uint32_t> graphics;
    std::optional<uint32_t> present;

    bool isComplete()
    {
        return graphics.has_value() && present.has_value();
    }
};

class VulkanPhysicalDevice {
public:
    VulkanPhysicalDevice(VulkanGraphicsContext* ctx);
    ~VulkanPhysicalDevice();

    static std::shared_ptr<VulkanPhysicalDevice> Select(VulkanGraphicsContext* ctx);
    static std::vector<VkPhysicalDevice> List(VulkanGraphicsContext* ctx);

    VkPhysicalDevice Raw() const { return m_PhysicalDevice; }
    VkPhysicalDeviceProperties2 GetProperties() const { return m_DeviceProps; }
    QueueFamilyIndex GetQueueFamilyIndices() const { return m_Indices; }

    bool IsExtensionSupported(const std::string& extension) const;

private:
    VkPhysicalDevice m_PhysicalDevice;
    VkPhysicalDeviceProperties2 m_DeviceProps;
    QueueFamilyIndex m_Indices;
};

class VulkanDevice {
public:
    VulkanDevice(std::shared_ptr<VulkanPhysicalDevice> physical_device, const VkPhysicalDeviceFeatures& features);
    ~VulkanDevice();
    void Destroy();

    VkDevice Raw() const { return m_Device; }
    std::shared_ptr<VulkanPhysicalDevice> GetPhysicalDevice() const { return m_PhysicalDevice; }
    VkQueue GetGraphicsQueue() const { return m_GraphicsQueue; }
    VkQueue GetPresentQueue() const { return m_PresentQueue; }

private:
    std::vector<const char*> GetRequiredExtensions();

private:
    VkDevice m_Device;
    std::shared_ptr<VulkanPhysicalDevice> m_PhysicalDevice;

    VkQueue m_GraphicsQueue;
    VkQueue m_PresentQueue;

    VkCommandPool m_CmdPool;
    // std::shared_mutex m_Mutex;
};
}
