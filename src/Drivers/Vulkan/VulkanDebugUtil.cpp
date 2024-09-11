#include "VulkanGraphicsContext.h"
#include "VulkanDebugUtil.h"

namespace Tabby {

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
    switch (messageSeverity) {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
        TB_CORE_TRACE("Vulkan validation layers: {}", pCallbackData->pMessage);
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
        TB_CORE_INFO("Vulkan validation layers: {}", pCallbackData->pMessage);
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        TB_CORE_WARN("Vulkan validation layers: {}", pCallbackData->pMessage);
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
        TB_CORE_ERROR("Vulkan validation layers: {}", pCallbackData->pMessage);
        break;
    default:
        break;
    }

    return VK_FALSE;
}

VulkanDebugUtils::VulkanDebugUtils(VulkanGraphicsContext* ctx)
    : m_Logger(VK_NULL_HANDLE)
{
    VkDebugUtilsMessengerCreateInfoEXT messenger_info = GetMessengerCreateInfo();

    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(ctx->GetVulkanInstance(), "vkCreateDebugUtilsMessengerEXT");
    VK_CHECK_RESULT(func(ctx->GetVulkanInstance(), &messenger_info, nullptr, &m_Logger));
}

VulkanDebugUtils::~VulkanDebugUtils()
{
}

void VulkanDebugUtils::Destroy(VulkanGraphicsContext* ctx)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(ctx->GetVulkanInstance(), "vkDestroyDebugUtilsMessengerEXT");
    func(ctx->GetVulkanInstance(), m_Logger, nullptr);
}

VkDebugUtilsMessengerCreateInfoEXT VulkanDebugUtils::GetMessengerCreateInfo()
{
    VkDebugUtilsMessengerCreateInfoEXT messenger_info = {};
    messenger_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    messenger_info.pfnUserCallback = debugCallback;
    // messenger_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
    // messenger_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    messenger_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    messenger_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

    return messenger_info;
}
}
