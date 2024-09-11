#pragma once

#include "VulkanCommon.h"

namespace Tabby {

class VulkanGraphicsContext;

class VulkanDebugUtils {
public:
    VulkanDebugUtils(VulkanGraphicsContext* ctx);
    ~VulkanDebugUtils();

    void Destroy(VulkanGraphicsContext* ctx);

    static VkDebugUtilsMessengerCreateInfoEXT GetMessengerCreateInfo();

private:
    VkDebugUtilsMessengerEXT m_Logger;
};
}
