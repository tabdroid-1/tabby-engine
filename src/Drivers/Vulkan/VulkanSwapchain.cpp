#include "Tabby/Core/Assert.h"
#include "VulkanGraphicsContext.h"
#include "VulkanDeviceCmdBuffer.h"
#include "VulkanRenderPass.h"
#include "VulkanSwapchain.h"
#include "VulkanDevice.h"
#include <Drivers/Vulkan/VulkanImage.h>

#include <Tabby/Core/Application.h>

#include <SDL_vulkan.h>
#include <SDL.h>

namespace Tabby {

VulkanSwapchain::VulkanSwapchain(const SwapchainSpecification& spec)
    : m_Specification(spec)
    , m_Swapchain(VK_NULL_HANDLE)
    , m_Surface(VK_NULL_HANDLE)
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
    TB_CORE_ASSERT_TAGGED(success, "Failed to create window surface!");

    // Looking for available presentation modes.
    // FIFO (aka v-synced) is guaranteed to be available, so only looking for Mailbox (non v-synced) mode.
    uint32_t present_mode_count = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device->GetPhysicalDevice()->Raw(), m_Surface, &present_mode_count, nullptr);

    std::vector<VkPresentModeKHR> present_modes(present_mode_count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device->GetPhysicalDevice()->Raw(), m_Surface, &present_mode_count, present_modes.data());

    m_SupportsMailboxPresentation = false;
    for (auto& mode : present_modes) {
        if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
            m_SupportsMailboxPresentation = true;
    }

    // Looking for suitable surface color space
    uint32_t surface_format_count = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device->GetPhysicalDevice()->Raw(), m_Surface, &surface_format_count, nullptr);

    std::vector<VkSurfaceFormatKHR> surface_formats(surface_format_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device->GetPhysicalDevice()->Raw(), m_Surface, &surface_format_count, surface_formats.data());

    // To begin, set surface format as first available, if no srgb 32-bit format is supported.
    m_SurfaceFormat = surface_formats[0];

    for (auto& format : surface_formats) {
        if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            m_SurfaceFormat = format;
    }
}

void VulkanSwapchain::CreateSwapchain()
{
    auto device = VulkanGraphicsContext::Get()->GetDevice();

    // m_Specification = spec;
    m_Images.reserve(m_SwachainImageCount);
    m_Semaphores.reserve(VulkanGraphicsContext::GetFramesInFlight());

    if (m_Swapchain) [[likely]] {
        vkDestroySwapchainKHR(device->Raw(), m_Swapchain, nullptr);
    }

    // UIntVector2 extent = (UIntVector2)extent;
    // if (extent.x + extent.y == 0) {
    //     extent = { 1, 1 };
    // }

    IntVector2 extent;

    // SDL_Vulkan_GetDrawableSize((SDL_Window*)Application::GetWindow().GetNativeWindow(), &width, &height);
    SDL_GetWindowSizeInPixels((SDL_Window*)Application::GetWindow().GetNativeWindow(), &extent.x, &extent.y);

    VkSurfaceCapabilitiesKHR surface_capabilities = {};
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device->GetPhysicalDevice()->Raw(), m_Surface, &surface_capabilities);

    VkSwapchainCreateInfoKHR swapchain_create_info = {};
    swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchain_create_info.surface = m_Surface;
    swapchain_create_info.oldSwapchain = VK_NULL_HANDLE;
    swapchain_create_info.imageFormat = m_SurfaceFormat.format;
    swapchain_create_info.imageColorSpace = m_SurfaceFormat.colorSpace;
    swapchain_create_info.minImageCount = 3;
    swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchain_create_info.imageExtent = { (uint32_t)extent.x, (uint32_t)extent.y };
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

    m_Images.reserve(m_SwachainImageCount);
    std::vector<VkImage> pure_swapchain_images(m_SwachainImageCount);

    VK_CHECK_RESULT(vkGetSwapchainImagesKHR(device->Raw(), m_Swapchain, (uint32_t*)&m_SwachainImageCount, pure_swapchain_images.data()));

    VulkanDeviceCmdBuffer image_layout_transition_command_buffer = device->AllocateTransientCmdBuffer();

    for (auto& image : m_Images) {
        vkDestroyImageView(device->Raw(), image->RawView(), nullptr);
    }

    m_Images.clear();

    for (auto& image : pure_swapchain_images) {
        VkImageView image_view = VK_NULL_HANDLE;

        VkImageViewCreateInfo image_view_create_info = {};
        image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        image_view_create_info.image = image;
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

        VK_CHECK_RESULT(vkCreateImageView(device->Raw(), &image_view_create_info, nullptr, &image_view));

        ImageSpecification swapchain_image_spec = {};
        swapchain_image_spec.extent = { (uint32_t)m_Specification.extent.x, (uint32_t)m_Specification.extent.y, 1 };
        swapchain_image_spec.usage = ImageUsage::RENDER_TARGET;
        swapchain_image_spec.type = ImageType::TYPE_2D;
        swapchain_image_spec.format = convert(m_SurfaceFormat.format);

        m_Images.push_back(std::make_shared<VulkanImage>(swapchain_image_spec, image, image_view));

        VkImageMemoryBarrier image_memory_barrier = {};
        image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        image_memory_barrier.image = image;
        image_memory_barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        image_memory_barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        image_memory_barrier.subresourceRange.baseArrayLayer = 0;
        image_memory_barrier.subresourceRange.layerCount = 1;
        image_memory_barrier.subresourceRange.baseMipLevel = 0;
        image_memory_barrier.subresourceRange.levelCount = 1;
        // image_memory_barrier.srcAccessMask = 0;
        // image_memory_barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;

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

    for (auto& image : m_Images) {
        image->SetCurrentLayout(ImageLayout::PRESENT_SRC);
    }

    m_CurrentFrameIndex = 0;

    /*  ===================
     *	Create sync objects
     */

    VkSemaphoreCreateInfo semaphore_create_info = {};
    semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    for (int i = 0; i < m_Specification.frames_in_flight; i++) {
        VkSemaphore render_semaphore;
        VkSemaphore present_semaphore;

        VK_CHECK_RESULT(vkCreateSemaphore(device->Raw(), &semaphore_create_info, nullptr, &render_semaphore));
        VK_CHECK_RESULT(vkCreateSemaphore(device->Raw(), &semaphore_create_info, nullptr, &present_semaphore));

        m_Semaphores.push_back({ render_semaphore, present_semaphore });
    }

    m_Fences.reserve(VulkanGraphicsContext::GetFramesInFlight());

    m_Semaphores.shrink_to_fit();
    m_Fences.shrink_to_fit();

    VkFenceCreateInfo fence_create_info = {};
    fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (int i = 0; i < VulkanGraphicsContext::GetFramesInFlight(); i++) {
        VkFence fence;
        VK_CHECK_RESULT(vkCreateFence(device->Raw(), &fence_create_info, nullptr, &fence));
        m_Fences.push_back(fence);
    }
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
    for (auto& image : m_Images) {
        vkDestroyImageView(device->Raw(), image->RawView(), nullptr);
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

void VulkanSwapchain::SetVSync(bool vsync)
{
    if (m_Specification.vsync == vsync)
        return;

    m_Specification.vsync = vsync;

    CreateSwapchain();
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
        VkSurfaceCapabilitiesKHR surface_capabilities = {};
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device->GetPhysicalDevice()->Raw(), m_Surface, &surface_capabilities);

        m_Specification.extent = { (int)surface_capabilities.currentExtent.width, (int)surface_capabilities.currentExtent.height };

        vkQueueWaitIdle(device->GetPresentQueue());

        CreateSwapchain();
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

    VkResult present_result = vkQueuePresentKHR(device->GetGraphicsQueue(), &present_info);

    if (present_result == VK_ERROR_OUT_OF_DATE_KHR || present_result == VK_SUBOPTIMAL_KHR) {
        VkSurfaceCapabilitiesKHR surface_capabilities = {};
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device->GetPhysicalDevice()->Raw(), m_Surface, &surface_capabilities);

        m_Specification.extent = { (int)surface_capabilities.currentExtent.width, (int)surface_capabilities.currentExtent.height };

        vkQueueWaitIdle(device->GetGraphicsQueue());

        SwapchainSpecification new_spec = GetSpecification();
        new_spec.extent = { surface_capabilities.currentExtent.width, surface_capabilities.currentExtent.height };

        CreateSwapchain();
    }

    m_CurrentFrameIndex = (m_CurrentImageIndex + 1) % m_Specification.frames_in_flight;
}

// VulkanSwapchain::VulkanSwapchain()
//     : m_Swapchain(VK_NULL_HANDLE)
// {
// }
//
// VulkanSwapchain::~VulkanSwapchain()
// {
// }
//
// void VulkanSwapchain::CreateSurface()
// {
//     auto device = VulkanGraphicsContext::Get()->GetDevice();
//
//     VulkanGraphicsContext* ctx = VulkanGraphicsContext::Get();
//     bool success = SDL_Vulkan_CreateSurface((SDL_Window*)Application::GetWindow().GetNativeWindow(), ctx->GetVulkanInstance(), &m_Surface);
//     TB_CORE_ASSERT_TAGGED(success, "Failed to create window surface!");
//
//     uint32_t present_mode_count = 0;
//     vkGetPhysicalDeviceSurfacePresentModesKHR(device->GetPhysicalDevice()->Raw(), m_Surface, &present_mode_count, nullptr);
//
//     std::vector<VkPresentModeKHR> present_modes(present_mode_count);
//     vkGetPhysicalDeviceSurfacePresentModesKHR(device->GetPhysicalDevice()->Raw(), m_Surface, &present_mode_count, present_modes.data());
//
//     m_SupportsMailboxPresentation = false;
//     for (auto& mode : present_modes) {
//         if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
//             m_SupportsMailboxPresentation = true;
//     }
//
//     uint32_t surface_format_count = 0;
//     vkGetPhysicalDeviceSurfaceFormatsKHR(device->GetPhysicalDevice()->Raw(), m_Surface, &surface_format_count, nullptr);
//
//     std::vector<VkSurfaceFormatKHR> surface_formats(surface_format_count);
//     vkGetPhysicalDeviceSurfaceFormatsKHR(device->GetPhysicalDevice()->Raw(), m_Surface, &surface_format_count, surface_formats.data());
//
//     m_SurfaceFormat = surface_formats[0];
//
//     for (auto& format : surface_formats) {
//         if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
//             m_SurfaceFormat = format;
//     }
// }
//
// void VulkanSwapchain::CreateSwapchain()
// {
//     auto device = VulkanGraphicsContext::Get()->GetDevice();
//
//     if (m_Swapchain) [[likely]] {
//         vkDestroySwapchainKHR(device->Raw(), m_Swapchain, nullptr);
//     }
//
//     VkSurfaceCapabilitiesKHR surface_capabilities = {};
//     vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device->GetPhysicalDevice()->Raw(), m_Surface, &surface_capabilities);
//
//     uint32_t surface_format_count = 0;
//     vkGetPhysicalDeviceSurfaceFormatsKHR(device->GetPhysicalDevice()->Raw(), m_Surface, &surface_format_count, nullptr);
//
//     std::vector<VkSurfaceFormatKHR> surface_formats(surface_format_count);
//     vkGetPhysicalDeviceSurfaceFormatsKHR(device->GetPhysicalDevice()->Raw(), m_Surface, &surface_format_count, surface_formats.data());
//
//     uint32_t present_mode_count = 0;
//     vkGetPhysicalDeviceSurfacePresentModesKHR(device->GetPhysicalDevice()->Raw(), m_Surface, &present_mode_count, nullptr);
//
//     std::vector<VkPresentModeKHR> present_modes(present_mode_count);
//     vkGetPhysicalDeviceSurfacePresentModesKHR(device->GetPhysicalDevice()->Raw(), m_Surface, &present_mode_count, present_modes.data());
//
//     for (auto& format : surface_formats) {
//         if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
//             m_SurfaceFormat = format;
//     }
//
//     m_CurrentPresentMode = VK_PRESENT_MODE_FIFO_KHR;
//     m_SupportsMailboxPresentation = false;
//     for (auto& mode : present_modes) {
//         if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
//             m_CurrentPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
//             m_SupportsMailboxPresentation = true;
//         }
//     }
//
//     int width, height;
//
//     // SDL_Vulkan_GetDrawableSize((SDL_Window*)Application::GetWindow().GetNativeWindow(), &width, &height);
//     SDL_GetWindowSizeInPixels((SDL_Window*)Application::GetWindow().GetNativeWindow(), &width, &height);
//
//     VkExtent2D actualExtent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
//
//     actualExtent.width = std::clamp(actualExtent.width, surface_capabilities.minImageExtent.width, surface_capabilities.maxImageExtent.width);
//     actualExtent.height = std::clamp(actualExtent.height, surface_capabilities.minImageExtent.height, surface_capabilities.maxImageExtent.height);
//     m_Specification.extent = { actualExtent.width, actualExtent.height };
//
//     VkSwapchainCreateInfoKHR createInfo {};
//     createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
//     createInfo.surface = m_Surface;
//
//     createInfo.minImageCount = 3;
//     createInfo.imageFormat = m_SurfaceFormat.format;
//     createInfo.imageColorSpace = m_SurfaceFormat.colorSpace;
//     createInfo.imageExtent = actualExtent;
//     createInfo.imageArrayLayers = 1;
//     createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
//
//     auto indices = VulkanGraphicsContext::Get()->GetDevice()->GetPhysicalDevice()->GetQueueFamilyIndices();
//     uint32_t queueFamilyIndices[] = { indices.graphics.value(),
//         indices.present.value() };
//
//     if (indices.graphics != indices.present) {
//         createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
//         createInfo.queueFamilyIndexCount = 2;
//         createInfo.pQueueFamilyIndices = queueFamilyIndices;
//     } else {
//         createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
//     }
//
//     createInfo.preTransform = surface_capabilities.currentTransform;
//     createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
//     createInfo.presentMode = m_CurrentPresentMode;
//     createInfo.clipped = VK_TRUE;
//
//     createInfo.oldSwapchain = VK_NULL_HANDLE;
//
//     if (vkCreateSwapchainKHR(VulkanGraphicsContext::Get()->GetDevice()->Raw(), &createInfo, nullptr, &m_Swapchain) != VK_SUCCESS) {
//         throw std::runtime_error("failed to create swap chain!");
//     }
//
//     VK_CHECK_RESULT(vkGetSwapchainImagesKHR(device->Raw(), m_Swapchain, (uint32_t*)&m_SwachainImageCount, nullptr));
//     std::vector<VkImage> pure_swapchain_images(m_SwachainImageCount);
//     VK_CHECK_RESULT(vkGetSwapchainImagesKHR(device->Raw(), m_Swapchain, (uint32_t*)&m_SwachainImageCount, pure_swapchain_images.data()));
//
//     VulkanDeviceCmdBuffer image_layout_transition_command_buffer = device->AllocateTransientCmdBuffer();
//     //
//     // for (auto& image : m_Images) {
//     //     vkDestroyImageView(device->Raw(), image->RawView(), nullptr);
//     // }
//
//     m_Images.clear();
//
//     for (auto& image : pure_swapchain_images) {
//         VkImageView image_view = VK_NULL_HANDLE;
//
//         VkImageViewCreateInfo image_view_create_info = {};
//         image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
//         image_view_create_info.image = image;
//         image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
//         image_view_create_info.format = m_SurfaceFormat.format;
//         image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
//         image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
//         image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
//         image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
//         image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
//         image_view_create_info.subresourceRange.baseArrayLayer = 0;
//         image_view_create_info.subresourceRange.layerCount = 1;
//         image_view_create_info.subresourceRange.baseMipLevel = 0;
//         image_view_create_info.subresourceRange.levelCount = 1;
//
//         VK_CHECK_RESULT(vkCreateImageView(device->Raw(), &image_view_create_info, nullptr, &image_view));
//
//         ImageSpecification swapchain_image_spec = {};
//         swapchain_image_spec.extent = { (uint32_t)m_Specification.extent.x, (uint32_t)m_Specification.extent.y, 1 };
//         swapchain_image_spec.usage = ImageUsage::RENDER_TARGET;
//         swapchain_image_spec.type = ImageType::TYPE_2D;
//         swapchain_image_spec.format = convert(m_SurfaceFormat.format);
//
//         m_Images.push_back(std::make_shared<VulkanImage>(swapchain_image_spec, image, image_view));
//
//         VkImageMemoryBarrier image_memory_barrier = {};
//         image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
//         image_memory_barrier.image = image;
//         image_memory_barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
//         image_memory_barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
//         image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
//         image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
//         image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
//         image_memory_barrier.subresourceRange.baseArrayLayer = 0;
//         image_memory_barrier.subresourceRange.layerCount = 1;
//         image_memory_barrier.subresourceRange.baseMipLevel = 0;
//         image_memory_barrier.subresourceRange.levelCount = 1;
//
//         vkCmdPipelineBarrier(
//             image_layout_transition_command_buffer,
//             VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
//             VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
//             0,
//             0,
//             nullptr,
//             0,
//             nullptr,
//             1,
//             &image_memory_barrier);
//     }
//
//     device->ExecuteTransientCmdBuffer(image_layout_transition_command_buffer, true);
//
//     for (auto& image : m_Images) {
//         image->SetCurrentLayout(ImageLayout::PRESENT_SRC);
//     }
//
//     m_CurrentFrameIndex = 0;
//
//     if (m_Semaphores.size() != VulkanGraphicsContext::GetFramesInFlight()) {
//
//         m_Semaphores.resize(VulkanGraphicsContext::GetFramesInFlight());
//
//         VkSemaphoreCreateInfo semaphore_create_info = {};
//         semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
//
//         for (int i = 0; i < VulkanGraphicsContext::GetFramesInFlight(); i++) {
//
//             VK_CHECK_RESULT(vkCreateSemaphore(device->Raw(), &semaphore_create_info, nullptr, &m_Semaphores[i].render_complete));
//             VK_CHECK_RESULT(vkCreateSemaphore(device->Raw(), &semaphore_create_info, nullptr, &m_Semaphores[i].present_complete));
//         }
//
//         m_Fences.resize(VulkanGraphicsContext::GetFramesInFlight());
//         VkFenceCreateInfo fence_create_info = {};
//         fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
//         fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
//
//         for (int i = 0; i < VulkanGraphicsContext::GetFramesInFlight(); i++) {
//             VK_CHECK_RESULT(vkCreateFence(device->Raw(), &fence_create_info, nullptr, &m_Fences[i]));
//         }
//     }
// }
//
// void VulkanSwapchain::DestroySwapchain()
// {
//     std::shared_ptr<VulkanDevice> device = VulkanGraphicsContext::Get()->GetDevice();
//
//     vkDeviceWaitIdle(device->Raw());
//
//     for (auto& image : m_Images) {
//         vkDestroyImageView(device->Raw(), image->RawView(), nullptr);
//     }
//
//     vkDestroySwapchainKHR(device->Raw(), m_Swapchain, nullptr);
//
//     for (auto& semaphores : m_Semaphores) {
//         vkDestroySemaphore(device->Raw(), semaphores.render_complete, nullptr);
//         vkDestroySemaphore(device->Raw(), semaphores.present_complete, nullptr);
//     }
//
//     for (auto& fence : m_Fences) {
//         vkDestroyFence(device->Raw(), fence, nullptr);
//     }
//
//     m_Swapchain = VK_NULL_HANDLE;
// }
//
// void VulkanSwapchain::DestroySurface()
// {
//     VulkanGraphicsContext* ctx = VulkanGraphicsContext::Get();
//
//     TB_CORE_ASSERT_TAGGED(!m_Swapchain, "Attempted to destroy window surface, but associated swapchain was not destroyed yet");
//     vkDestroySurfaceKHR(ctx->GetVulkanInstance(), m_Surface, nullptr);
// }
//
// void VulkanSwapchain::BeginFrame()
// {
//     auto device = VulkanGraphicsContext::Get()->GetDevice();
//
//     VK_CHECK_RESULT(vkWaitForFences(device->Raw(), 1, &m_Fences[m_CurrentFrameIndex], VK_TRUE, UINT64_MAX));
//     VK_CHECK_RESULT(vkResetFences(device->Raw(), 1, &m_Fences[m_CurrentFrameIndex]));
//
//     VkResult acquisition_result = vkAcquireNextImageKHR(
//         device->Raw(),
//         m_Swapchain,
//         UINT64_MAX,
//         m_Semaphores[m_CurrentFrameIndex].present_complete,
//         VK_NULL_HANDLE,
//         &m_CurrentImageIndex);
//
//     if (acquisition_result == VK_ERROR_OUT_OF_DATE_KHR) {
//
//         vkDeviceWaitIdle(device->Raw());
//
//         VkSurfaceCapabilitiesKHR surface_capabilities = {};
//         vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device->GetPhysicalDevice()->Raw(), m_Surface, &surface_capabilities);
//
//         SwapchainSpecification new_spec = GetSpecification();
//         new_spec.extent = { surface_capabilities.currentExtent.width, surface_capabilities.currentExtent.height };
//
//         CreateSwapchain();
//         VulkanGraphicsContext::Get()->GetRenderPass()->CreateFramebuffer();
//
//         vkAcquireNextImageKHR(
//             device->Raw(),
//             m_Swapchain,
//             UINT64_MAX,
//             m_Semaphores[m_CurrentFrameIndex].present_complete,
//             VK_NULL_HANDLE,
//             &m_CurrentImageIndex);
//     }
// }
//
// void VulkanSwapchain::EndFrame()
// {
//     auto device = VulkanGraphicsContext::Get()->GetDevice();
//
//     VkPresentInfoKHR present_info = {};
//     present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
//     present_info.pImageIndices = &m_CurrentImageIndex;
//     present_info.swapchainCount = 1;
//     present_info.pSwapchains = &m_Swapchain;
//     present_info.waitSemaphoreCount = 1;
//     present_info.pWaitSemaphores = &m_Semaphores[m_CurrentFrameIndex].render_complete;
//     present_info.pResults = nullptr;
//
//     VkResult present_result = vkQueuePresentKHR(device->GetPresentQueue(), &present_info);
//
//     if (present_result == VK_ERROR_OUT_OF_DATE_KHR || present_result == VK_SUBOPTIMAL_KHR) {
//
//         vkDeviceWaitIdle(device->Raw());
//
//         VkSurfaceCapabilitiesKHR surface_capabilities = {};
//         vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device->GetPhysicalDevice()->Raw(), m_Surface, &surface_capabilities);
//
//         SwapchainSpecification new_spec = GetSpecification();
//         new_spec.extent = { surface_capabilities.currentExtent.width, surface_capabilities.currentExtent.height };
//
//         CreateSwapchain();
//         VulkanGraphicsContext::Get()->GetRenderPass()->CreateFramebuffer();
//     }
//
//     m_CurrentFrameIndex = (m_CurrentImageIndex + 1) % VulkanGraphicsContext::GetFramesInFlight();
// }
}
