#include <tbpch.h>

#include "VulkanGraphicsContext.h"
#include "VulkanDeviceCmdBuffer.h"
#include "VulkanDevice.h"

#include <SDL_vulkan.h>
#include <SDL.h>

// ===============
// PHYSICAL DEVICE
// ===============

namespace Tabby {

std::shared_ptr<VulkanPhysicalDevice> VulkanPhysicalDevice::Select(VulkanGraphicsContext* ctx)
{
    return std::make_shared<VulkanPhysicalDevice>(ctx);
}

// TODO: check for device surface support
VulkanPhysicalDevice::VulkanPhysicalDevice(VulkanGraphicsContext* ctx)
    : m_PhysicalDevice(VK_NULL_HANDLE)
{
    VkInstance vk_instance = ctx->GetVulkanInstance();

    uint32_t physical_device_count = 0;
    vkEnumeratePhysicalDevices(vk_instance, &physical_device_count, nullptr);

    std::vector<VkPhysicalDevice> physical_devices(physical_device_count);
    vkEnumeratePhysicalDevices(vk_instance, &physical_device_count, physical_devices.data());

    uint32_t* device_scores = new uint32_t[physical_device_count];
    memset(device_scores, 0, sizeof(uint32_t) * physical_device_count); // init memory

    uint32_t index = 0;
    for (auto& device : physical_devices) {
        VkPhysicalDeviceProperties device_props = {};
        VkPhysicalDeviceMemoryProperties device_memory_props = {};

        vkGetPhysicalDeviceProperties(device, &device_props);
        vkGetPhysicalDeviceMemoryProperties(device, &device_memory_props);

        switch (device_props.deviceType) {
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
            device_scores[index] += 50;
            break;
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
            device_scores[index] += 15;
            break;
        default:
            break;
        }

        uint64_t device_memory_size = 0;
        for (int i = 0; i < device_memory_props.memoryHeapCount; i++) {
            device_memory_size += device_memory_props.memoryHeaps[i].size;
        }

        device_scores[index] += device_memory_size / 500'000'000;

        index++;
    }

    uint32_t suitable_device_index = 0;
    for (int i = 0; i < physical_device_count; i++) {
        if (device_scores[i] > device_scores[suitable_device_index])
            suitable_device_index = i;
    }

    delete[] device_scores;

    m_PhysicalDevice = physical_devices[suitable_device_index];

    uint32_t queue_family_property_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &queue_family_property_count, nullptr);

    std::vector<VkQueueFamilyProperties> queue_family_properties(queue_family_property_count);
    vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &queue_family_property_count, queue_family_properties.data());

    uint32_t queue_family_index = 0;
    for (auto& family_prop : queue_family_properties) {
        if (family_prop.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            m_Indices.graphics = queue_family_index;
        }

        // VkBool32 presentSupport = false;
        // vkGetPhysicalDeviceSurfaceSupportKHR(m_PhysicalDevice, queue_family_index, surface, &presentSupport);
        //
        // if (presentSupport) {
        //     m_Indices.present = queue_family_index;
        // }

        if (m_Indices.isComplete()) {
            break;
        }

        queue_family_index++;
    }

    m_Indices.present = m_Indices.graphics;

    m_DeviceProps = {};
    m_DeviceProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;

    vkGetPhysicalDeviceProperties2(m_PhysicalDevice, &m_DeviceProps);
    TB_CORE_TRACE("Selected Vulkan device: {}", m_DeviceProps.properties.deviceName);
}

VulkanPhysicalDevice::~VulkanPhysicalDevice()
{
}

bool VulkanPhysicalDevice::IsExtensionSupported(const std::string& extension) const
{
    uint32_t count = 0;
    vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &count, nullptr);

    std::vector<VkExtensionProperties> supported_extensions(count);
    vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &count, supported_extensions.data());

    for (const auto& supported_ext : supported_extensions) {
        if (!strcmp(supported_ext.extensionName, extension.c_str())) {
            return true;
        }
    }

    TB_CORE_ERROR("Vulkan device extension is not supported!,extension.c_str()");

    return false;
}

// ==============
// LOGICAL DEVICE
// ==============
VulkanDevice::VulkanDevice(std::shared_ptr<VulkanPhysicalDevice> physical_device, const VkPhysicalDeviceFeatures& features)
    : m_Device(VK_NULL_HANDLE)
    , m_PhysicalDevice(physical_device)
    , m_GraphicsQueue(VK_NULL_HANDLE)
    , m_PresentQueue(VK_NULL_HANDLE)
{

    VkPhysicalDeviceFeatures supported_device_features = {};
    vkGetPhysicalDeviceFeatures(m_PhysicalDevice->Raw(), &supported_device_features);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = { m_PhysicalDevice->GetQueueFamilyIndices().graphics.value(),
        m_PhysicalDevice->GetQueueFamilyIndices().present.value() };

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }
    std::vector<const char*> enabled_extensions = GetRequiredExtensions();

    VkPhysicalDeviceSynchronization2FeaturesKHR sync2Features = {};
    sync2Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES_KHR;
    sync2Features.synchronization2 = VK_TRUE;

    VkPhysicalDeviceFeatures2 deviceFeatures2 = {};
    deviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    deviceFeatures2.pNext = &sync2Features;

    VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamic_rendering_feature;
    dynamic_rendering_feature.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR;
    dynamic_rendering_feature.pNext = &deviceFeatures2;
    dynamic_rendering_feature.dynamicRendering = VK_TRUE;

    VkDeviceCreateInfo device_create_info = {};
    device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_create_info.pNext = &dynamic_rendering_feature;
    device_create_info.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    device_create_info.pQueueCreateInfos = queueCreateInfos.data();
    device_create_info.pEnabledFeatures = &features;
    device_create_info.ppEnabledExtensionNames = enabled_extensions.data();
    device_create_info.enabledExtensionCount = enabled_extensions.size();

    VK_CHECK_RESULT(vkCreateDevice(m_PhysicalDevice->Raw(), &device_create_info, nullptr, &m_Device));
    vkGetDeviceQueue(m_Device, m_PhysicalDevice->GetQueueFamilyIndices().graphics.value(), 0, &m_GraphicsQueue);
    vkGetDeviceQueue(m_Device, m_PhysicalDevice->GetQueueFamilyIndices().present.value(), 0, &m_PresentQueue);

    VkCommandPoolCreateInfo cmd_pool_create_info = {};
    cmd_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cmd_pool_create_info.queueFamilyIndex = m_PhysicalDevice->GetQueueFamilyIndices().graphics.value();
    cmd_pool_create_info.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;

    VK_CHECK_RESULT(vkCreateCommandPool(m_Device, &cmd_pool_create_info, nullptr, &m_CmdPool));
}

VulkanDevice::~VulkanDevice()
{
    TB_CORE_ASSERT_TAGGED(m_Device == VK_NULL_HANDLE, "Vulkan device was not destroyed yet. Please, call VulkanDevice::Destroy().");
}

void VulkanDevice::Destroy()
{
    vkDestroyCommandPool(m_Device, m_CmdPool, nullptr);
    vkDestroyDevice(m_Device, nullptr);
    m_Device = VK_NULL_HANDLE;
}

VulkanDeviceCmdBuffer VulkanDevice::AllocateTransientCmdBuffer()
{
    VulkanDeviceCmdBuffer cmd_buffer;
    cmd_buffer.Begin();

    return cmd_buffer;
}

void VulkanDevice::ExecuteTransientCmdBuffer(VulkanDeviceCmdBuffer cmd_buffer, bool wait /*= false*/) const
{
    cmd_buffer.End();
    cmd_buffer.Execute(true); // TODO: make such feature that cmd buffer can be launched asynchronously and will be self-destroyed on work finished
    cmd_buffer.Destroy();
}

std::vector<const char*> VulkanDevice::GetRequiredExtensions()
{
    std::vector<const char*> extensions;

    if (m_PhysicalDevice->IsExtensionSupported(VK_KHR_SWAPCHAIN_EXTENSION_NAME)) {
        extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    }

    if (m_PhysicalDevice->IsExtensionSupported(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME)) {
        extensions.push_back(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME);
    }

    if (m_PhysicalDevice->IsExtensionSupported(VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME)) {
        extensions.push_back(VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME);
    }

#ifdef TB_PLATFORM_MACOS
    extensions.emplace_back("VK_KHR_portability_subset");
#endif

    TB_CORE_TRACE("Enabled Vulkan device extensions:");
    for (auto& ext : extensions) {
        TB_CORE_TRACE("\t{}", ext);
    }

    return extensions;
}
}
