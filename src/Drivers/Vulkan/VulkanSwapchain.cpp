#include "VulkanGraphicsContext.h"
#include "VulkanDeviceCmdBuffer.h"
#include "VulkanRenderPass.h"
#include "VulkanSwapchain.h"
#include "VulkanDevice.h"

#include <Tabby/Core/Application.h>

#include <SDL_vulkan.h>
#include <SDL.h>

namespace Tabby {

VulkanSwapchain::VulkanSwapchain()
    : m_Swapchain(VK_NULL_HANDLE)
{
}

VulkanSwapchain::~VulkanSwapchain()
{
}

void VulkanSwapchain::CreateSurface()
{
    auto device = VulkanGraphicsContext::Get()->GetDevice();

    VulkanGraphicsContext* ctx = VulkanGraphicsContext::Get();
    bool success = SDL_Vulkan_CreateSurface((SDL_Window*)Application::GetWindow().GetNativeWindow(), ctx->GetVulkanInstance(), &m_Surface);
    if (!success) {
        throw std::runtime_error("failed to create window surface!");
    }

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

void VulkanSwapchain::CreateSwapchain()
{
    auto device = VulkanGraphicsContext::Get()->GetDevice();

    if (m_Swapchain) [[likely]] {
        vkDestroySwapchainKHR(device->Raw(), m_Swapchain, nullptr);
    }

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device->GetPhysicalDevice()->Raw(), m_Surface, &m_SurfaceCapabilities);

    uint32_t surface_format_count = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device->GetPhysicalDevice()->Raw(), m_Surface, &surface_format_count, nullptr);

    std::vector<VkSurfaceFormatKHR> surface_formats(surface_format_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device->GetPhysicalDevice()->Raw(), m_Surface, &surface_format_count, surface_formats.data());

    uint32_t present_mode_count = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device->GetPhysicalDevice()->Raw(), m_Surface, &present_mode_count, nullptr);

    std::vector<VkPresentModeKHR> present_modes(present_mode_count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device->GetPhysicalDevice()->Raw(), m_Surface, &present_mode_count, present_modes.data());

    for (auto& format : surface_formats) {
        if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            m_SurfaceFormat = format;
    }

    m_CurrentPresentMode = VK_PRESENT_MODE_FIFO_KHR;
    m_SupportsMailboxPresentation = false;
    for (auto& mode : present_modes) {
        if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
            m_CurrentPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
            m_SupportsMailboxPresentation = true;
        }
    }

    int width, height;

    SDL_Vulkan_GetDrawableSize((SDL_Window*)Application::GetWindow().GetNativeWindow(), &width, &height);

    VkExtent2D actualExtent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };

    actualExtent.width = std::clamp(actualExtent.width, m_SurfaceCapabilities.minImageExtent.width, m_SurfaceCapabilities.maxImageExtent.width);
    actualExtent.height = std::clamp(actualExtent.height, m_SurfaceCapabilities.minImageExtent.height, m_SurfaceCapabilities.maxImageExtent.height);
    m_Extent = actualExtent;

    uint32_t imageCount = m_SurfaceCapabilities.minImageCount + 1;
    if (m_SurfaceCapabilities.maxImageCount > 0 && imageCount > m_SurfaceCapabilities.maxImageCount) {
        imageCount = m_SurfaceCapabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = m_Surface;

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = m_SurfaceFormat.format;
    createInfo.imageColorSpace = m_SurfaceFormat.colorSpace;
    createInfo.imageExtent = m_Extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    auto indices = VulkanGraphicsContext::Get()->GetDevice()->GetPhysicalDevice()->GetQueueFamilyIndices();
    uint32_t queueFamilyIndices[] = { indices.graphics.value(),
        indices.present.value() };

    if (indices.graphics != indices.present) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    createInfo.preTransform = m_SurfaceCapabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = m_CurrentPresentMode;
    createInfo.clipped = VK_TRUE;

    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(VulkanGraphicsContext::Get()->GetDevice()->Raw(), &createInfo, nullptr, &m_Swapchain) != VK_SUCCESS) {
        throw std::runtime_error("failed to create swap chain!");
    }

    vkGetSwapchainImagesKHR(VulkanGraphicsContext::Get()->GetDevice()->Raw(), m_Swapchain, &imageCount, nullptr);
    m_Images.resize(imageCount);
    vkGetSwapchainImagesKHR(VulkanGraphicsContext::Get()->GetDevice()->Raw(), m_Swapchain, &imageCount, m_Images.data());

    m_ImageFormat = m_SurfaceFormat.format;

    m_ImageViews.resize(m_Images.size());

    for (size_t i = 0; i < m_Images.size(); i++) {
        if (m_ImageViews[i])
            vkDestroyImageView(device->Raw(), m_ImageViews[i], nullptr);
    }

    VulkanDeviceCmdBuffer image_layout_transition_command_buffer = device->AllocateTransientCmdBuffer();

    for (size_t i = 0; i < m_Images.size(); i++) {
        VkImageViewCreateInfo createInfo {};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = m_Images[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = m_ImageFormat;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(VulkanGraphicsContext::Get()->GetDevice()->Raw(), &createInfo, nullptr, &m_ImageViews[i])
            != VK_SUCCESS) {
            throw std::runtime_error("failed to create image views!");
        }
        VkImageMemoryBarrier image_memory_barrier = {};
        image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        image_memory_barrier.image = m_Images[i];
        image_memory_barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        image_memory_barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        image_memory_barrier.subresourceRange.baseArrayLayer = 0;
        image_memory_barrier.subresourceRange.layerCount = 1;
        image_memory_barrier.subresourceRange.baseMipLevel = 0;
        image_memory_barrier.subresourceRange.levelCount = 1;

        vkCmdPipelineBarrier(
            image_layout_transition_command_buffer,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            0,
            0,
            nullptr,
            0,
            nullptr,
            1,
            &image_memory_barrier);
    }
    device->ExecuteTransientCmdBuffer(image_layout_transition_command_buffer, true);

    if (m_Semaphores.size() != VulkanGraphicsContext::GetFramesInFlight()) {

        m_Semaphores.resize(VulkanGraphicsContext::GetFramesInFlight());

        VkSemaphoreCreateInfo semaphore_create_info = {};
        semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        for (int i = 0; i < VulkanGraphicsContext::GetFramesInFlight(); i++) {

            VK_CHECK_RESULT(vkCreateSemaphore(device->Raw(), &semaphore_create_info, nullptr, &m_Semaphores[i].render_complete));
            VK_CHECK_RESULT(vkCreateSemaphore(device->Raw(), &semaphore_create_info, nullptr, &m_Semaphores[i].present_complete));
        }

        m_Fences.resize(VulkanGraphicsContext::GetFramesInFlight());
        VkFenceCreateInfo fence_create_info = {};
        fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (int i = 0; i < VulkanGraphicsContext::GetFramesInFlight(); i++) {
            VK_CHECK_RESULT(vkCreateFence(device->Raw(), &fence_create_info, nullptr, &m_Fences[i]));
        }
    }
}

void VulkanSwapchain::DestroySwapchain()
{
    std::shared_ptr<VulkanDevice> device = VulkanGraphicsContext::Get()->GetDevice();

    vkDeviceWaitIdle(device->Raw());

    for (auto& image_view : m_ImageViews) {
        vkDestroyImageView(device->Raw(), image_view, nullptr);
    }

    vkDestroySwapchainKHR(device->Raw(), m_Swapchain, nullptr);

    for (auto& semaphores : m_Semaphores) {
        vkDestroySemaphore(device->Raw(), semaphores.render_complete, nullptr);
        vkDestroySemaphore(device->Raw(), semaphores.present_complete, nullptr);
    }

    for (auto& fence : m_Fences) {
        vkDestroyFence(device->Raw(), fence, nullptr);
    }

    m_Swapchain = VK_NULL_HANDLE;
}

void VulkanSwapchain::DestroySurface()
{
    VulkanGraphicsContext* ctx = VulkanGraphicsContext::Get();

    if (!m_Swapchain)
        throw std::runtime_error("Attempted to destroy window surface, but associated swapchain was not destroyed yet");
    vkDestroySurfaceKHR(ctx->GetVulkanInstance(), m_Surface, nullptr);
}

void VulkanSwapchain::BeginFrame()
{
    auto device = VulkanGraphicsContext::Get()->GetDevice();

    VK_CHECK_RESULT(vkWaitForFences(device->Raw(), 1, &m_Fences[m_CurrentFrameIndex], VK_TRUE, UINT64_MAX));
    VK_CHECK_RESULT(vkResetFences(device->Raw(), 1, &m_Fences[m_CurrentFrameIndex]));

    VkResult acquisition_result = vkAcquireNextImageKHR(
        device->Raw(),
        m_Swapchain,
        UINT64_MAX,
        m_Semaphores[m_CurrentFrameIndex].present_complete,
        VK_NULL_HANDLE,
        &m_CurrentImageIndex);

    if (acquisition_result == VK_ERROR_OUT_OF_DATE_KHR) {

        vkDeviceWaitIdle(device->Raw());

        VkSurfaceCapabilitiesKHR surface_capabilities = {};
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device->GetPhysicalDevice()->Raw(), m_Surface, &surface_capabilities);

        SwapchainSpecification new_spec = GetSpecification();
        new_spec.width = surface_capabilities.currentExtent.width;
        new_spec.height = surface_capabilities.currentExtent.height;

        CreateSwapchain();
        VulkanGraphicsContext::Get()->GetRenderPass()->CreateFramebuffer();

        vkAcquireNextImageKHR(
            device->Raw(),
            m_Swapchain,
            UINT64_MAX,
            m_Semaphores[m_CurrentFrameIndex].present_complete,
            VK_NULL_HANDLE,
            &m_CurrentImageIndex);
    }
}

void VulkanSwapchain::EndFrame()
{
    auto device = VulkanGraphicsContext::Get()->GetDevice();

    VkPresentInfoKHR present_info = {};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.pImageIndices = &m_CurrentImageIndex;
    present_info.swapchainCount = 1;
    present_info.pSwapchains = &m_Swapchain;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = &m_Semaphores[m_CurrentFrameIndex].render_complete;
    present_info.pResults = nullptr;

    VkResult present_result = vkQueuePresentKHR(device->GetPresentQueue(), &present_info);

    if (present_result == VK_ERROR_OUT_OF_DATE_KHR || present_result == VK_SUBOPTIMAL_KHR) {

        vkDeviceWaitIdle(device->Raw());

        VkSurfaceCapabilitiesKHR surface_capabilities = {};
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device->GetPhysicalDevice()->Raw(), m_Surface, &surface_capabilities);

        SwapchainSpecification new_spec = GetSpecification();
        new_spec.width = surface_capabilities.currentExtent.width;
        new_spec.height = surface_capabilities.currentExtent.height;

        CreateSwapchain();
        VulkanGraphicsContext::Get()->GetRenderPass()->CreateFramebuffer();
    }

    m_CurrentFrameIndex = (m_CurrentImageIndex + 1) % VulkanGraphicsContext::GetFramesInFlight();
}
}
