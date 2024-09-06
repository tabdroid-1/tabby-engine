#include <Drivers/Vulkan/VulkanGraphicsContext.h>
#include <Drivers/Vulkan/VulkanSwapchain.h>
#include <Drivers/Vulkan/VulkanDevice.h>
#include <Tabby/Core/Application.h>

#include <SDL_vulkan.h>
#include <SDL.h>

namespace Tabby {

VulkanSwapchain::VulkanSwapchain(const SwapchainSpecification& spec)
    : m_Specification(spec)
    , m_Swapchain(VK_NULL_HANDLE)
{
}

VulkanSwapchain::~VulkanSwapchain()
{
}

void VulkanSwapchain::CreateSurface(const SwapchainSpecification& spec)
{

    auto device = VulkanGraphicsContext::Get()->GetDevice();

    VulkanGraphicsContext* ctx = VulkanGraphicsContext::Get();
    bool success = SDL_Vulkan_CreateSurface((SDL_Window*)Application::GetWindow().GetNativeWindow(), ctx->GetVulkanInstance(), &m_Surface);
    TB_CORE_VERIFY_TAGGED(success, "SDL failed to create surface.");
    TB_CORE_TRACE("Created window surface");

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device->GetPhysicalDevice()->Raw(), m_Surface, &m_SurfaceCapabilities);

    uint32_t present_mode_count = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device->GetPhysicalDevice()->Raw(), m_Surface, &present_mode_count, nullptr);

    std::vector<VkPresentModeKHR> present_modes(present_mode_count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device->GetPhysicalDevice()->Raw(), m_Surface, &present_mode_count, present_modes.data());

    m_SupportsMailboxPresentation = false;
    for (auto& mode : present_modes) {
        if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
            m_SupportsMailboxPresentation = true;
    }

    uint32_t surface_format_count = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device->GetPhysicalDevice()->Raw(), m_Surface, &surface_format_count, nullptr);

    std::vector<VkSurfaceFormatKHR> surface_formats(surface_format_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device->GetPhysicalDevice()->Raw(), m_Surface, &surface_format_count, surface_formats.data());

    m_SurfaceFormat = surface_formats[0];

    for (auto& format : surface_formats) {
        if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            m_SurfaceFormat = format;
    }
}

void VulkanSwapchain::CreateSwapchain(const SwapchainSpecification& spec)
{
    auto device = VulkanGraphicsContext::Get()->GetDevice();

    m_Specification = spec;
    // m_Semaphores.reserve(spec.frames_in_flight);

    if (m_Swapchain) [[likely]] {
        vkDestroySwapchainKHR(device->Raw(), m_Swapchain, nullptr);
    }

    UIntVector2 extent = (UIntVector2)spec.extent;
    if (extent.x + extent.y == 0) {
        extent = { 1, 1 };
    }

    VkSurfaceCapabilitiesKHR surface_capabilities = {};
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device->GetPhysicalDevice()->Raw(), m_Surface, &surface_capabilities);

    extent.x = std::clamp(extent.x, surface_capabilities.minImageExtent.width, surface_capabilities.maxImageExtent.width);
    extent.y = std::clamp(extent.y, surface_capabilities.minImageExtent.height, surface_capabilities.maxImageExtent.height);

    m_Specification.extent = extent;

    VkSwapchainCreateInfoKHR swapchain_create_info = {};
    swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchain_create_info.surface = m_Surface;
    swapchain_create_info.oldSwapchain = VK_NULL_HANDLE;
    swapchain_create_info.imageFormat = m_SurfaceFormat.format;
    swapchain_create_info.imageColorSpace = m_SurfaceFormat.colorSpace;
    swapchain_create_info.minImageCount = 3;
    swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchain_create_info.imageExtent = { extent.x, extent.y };
    swapchain_create_info.imageArrayLayers = 1;
    swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchain_create_info.clipped = VK_TRUE;
    swapchain_create_info.imageUsage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchain_create_info.preTransform = surface_capabilities.currentTransform;
    swapchain_create_info.presentMode = VK_PRESENT_MODE_FIFO_KHR;

    if (m_SupportsMailboxPresentation) {
        swapchain_create_info.presentMode = m_Specification.vsync ? VK_PRESENT_MODE_FIFO_KHR : VK_PRESENT_MODE_MAILBOX_KHR;
    }

    VK_CHECK_RESULT(vkCreateSwapchainKHR(device->Raw(), &swapchain_create_info, nullptr, &m_Swapchain));

    VK_CHECK_RESULT(vkGetSwapchainImagesKHR(device->Raw(), m_Swapchain, (uint32_t*)&m_SwachainImageCount, nullptr));
    m_SwapchainImages.reserve(m_SwachainImageCount);
    m_SwapchainImageViews.reserve(m_SwachainImageCount);
    VK_CHECK_RESULT(vkGetSwapchainImagesKHR(device->Raw(), m_Swapchain, (uint32_t*)&m_SwachainImageCount, m_SwapchainImages.data()));

    for (size_t i = 0; i < m_SwapchainImages.size(); i++) {
        VkImageViewCreateInfo image_view_create_info = {};
        image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        image_view_create_info.image = m_SwapchainImages[i];
        image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        image_view_create_info.format = m_SurfaceFormat.format;
        image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        image_view_create_info.subresourceRange.baseArrayLayer = 0;
        image_view_create_info.subresourceRange.layerCount = 1;
        image_view_create_info.subresourceRange.baseMipLevel = 0;
        image_view_create_info.subresourceRange.levelCount = 1;

        VK_CHECK_RESULT(vkCreateImageView(device->Raw(), &image_view_create_info, nullptr, &m_SwapchainImageViews[i]));
    }

    // CREATE RENDERPASS

    VkSemaphoreCreateInfo semaphore_create_info = {};
    semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkSemaphore render_semaphore;
    VkSemaphore present_semaphore;

    VK_CHECK_RESULT(vkCreateSemaphore(device->Raw(), &semaphore_create_info, nullptr, &render_semaphore));
    VK_CHECK_RESULT(vkCreateSemaphore(device->Raw(), &semaphore_create_info, nullptr, &present_semaphore));
    m_Semaphore = { render_semaphore, present_semaphore };

    VkFenceCreateInfo fence_create_info = {};
    fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    VK_CHECK_RESULT(vkCreateFence(device->Raw(), &fence_create_info, nullptr, &m_Fence));

    TB_CORE_TRACE(
        "Created renderer swapchain. Spec - extent: {0}x{1}, VSync: {2}, image count: {3}",
        spec.extent.x,
        spec.extent.y,
        spec.vsync ? "on" : "off",
        m_SwachainImageCount);
}

void VulkanSwapchain::DestroySurface()
{
    VulkanGraphicsContext* ctx = VulkanGraphicsContext::Get();

    TB_CORE_ASSERT_TAGGED(!m_Swapchain, "Attempted to destroy window surface, but associated swapchain was not destroyed yet");
    vkDestroySurfaceKHR(ctx->GetVulkanInstance(), m_Surface, nullptr);
}

void VulkanSwapchain::DestroySwapchain()
{
    Shared<VulkanDevice> device = VulkanGraphicsContext::Get()->GetDevice();

    vkDeviceWaitIdle(device->Raw());

    for (auto& image_view : m_SwapchainImageViews) {
        vkDestroyImageView(device->Raw(), image_view, nullptr);
    }

    vkDestroySwapchainKHR(device->Raw(), m_Swapchain, nullptr);

    vkDestroySemaphore(device->Raw(), m_Semaphore.render_complete, nullptr);
    vkDestroySemaphore(device->Raw(), m_Semaphore.present_complete, nullptr);

    vkDestroyFence(device->Raw(), m_Fence, nullptr);

    m_Swapchain = VK_NULL_HANDLE;
}

void VulkanSwapchain::WaitFence()
{
    Shared<VulkanDevice> device = VulkanGraphicsContext::Get()->GetDevice();

    VK_CHECK_RESULT(vkWaitForFences(device->Raw(), 1, &m_Fence, VK_TRUE, UINT64_MAX));
    VK_CHECK_RESULT(vkResetFences(device->Raw(), 1, &m_Fence));
}

void VulkanSwapchain::BeginFrame()
{
    Shared<VulkanDevice> device = VulkanGraphicsContext::Get()->GetDevice();

    VkResult acquisition_result = vkAcquireNextImageKHR(
        device->Raw(),
        m_Swapchain,
        UINT64_MAX,
        m_Semaphore.present_complete,
        VK_NULL_HANDLE,
        &m_CurrentImageIndex);

    if (acquisition_result == VK_ERROR_OUT_OF_DATE_KHR || acquisition_result == VK_SUBOPTIMAL_KHR) {
        VkSurfaceCapabilitiesKHR surface_capabilities = {};
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device->GetPhysicalDevice()->Raw(), m_Surface, &surface_capabilities);

        SwapchainSpecification new_spec = GetSpecification();
        new_spec.extent = { (int32_t)surface_capabilities.currentExtent.width, (int32_t)surface_capabilities.currentExtent.height };

        vkQueueWaitIdle(device->GetAsyncComputeQueue());

        CreateSwapchain(new_spec);
    }
}

void VulkanSwapchain::EndFrame()
{
    Shared<VulkanDevice> device = VulkanGraphicsContext::Get()->GetDevice();

    VkPresentInfoKHR present_info = {};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.pImageIndices = &m_CurrentImageIndex;
    present_info.swapchainCount = 1;
    present_info.pSwapchains = &m_Swapchain;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = &m_Semaphore.render_complete;
    present_info.pResults = nullptr;

    VkResult present_result = vkQueuePresentKHR(device->GetGeneralQueue(), &present_info);

    if (present_result == VK_ERROR_OUT_OF_DATE_KHR || present_result == VK_SUBOPTIMAL_KHR) {
        VkSurfaceCapabilitiesKHR surface_capabilities = {};
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device->GetPhysicalDevice()->Raw(), m_Surface, &surface_capabilities);

        SwapchainSpecification new_spec = GetSpecification();
        new_spec.extent = { (int32_t)surface_capabilities.currentExtent.width, (int32_t)surface_capabilities.currentExtent.height };

        vkQueueWaitIdle(device->GetGeneralQueue());

        CreateSwapchain(new_spec);
    }
    m_CurrentFrameIndex = (m_CurrentImageIndex + 1) % m_Specification.frames_in_flight;
}

}
