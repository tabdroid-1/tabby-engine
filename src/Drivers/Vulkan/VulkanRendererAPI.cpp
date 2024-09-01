#include <Drivers/Vulkan/VulkanGraphicsContext.h>
#include <Drivers/Vulkan/VulkanRendererAPI.h>
#include <Drivers/Vulkan/VulkanDevice.h>

#include <imgui.h>

#include "backends/imgui_impl_vulkan.h"

namespace Tabby {

VulkanRendererAPI::VulkanRendererAPI(const RendererConfig& config)
    : m_Config(config)
{
    m_GraphicsContext = std::make_shared<VulkanGraphicsContext>(config);
    m_Device = m_GraphicsContext->GetDevice();

    auto base_swapchain = m_GraphicsContext->GetSwapchain();
    m_Swapchain = ShareAs<VulkanSwapchain>(base_swapchain);
}

VulkanRendererAPI::~VulkanRendererAPI()
{
    vkDeviceWaitIdle(m_Device->Raw());
}

}
