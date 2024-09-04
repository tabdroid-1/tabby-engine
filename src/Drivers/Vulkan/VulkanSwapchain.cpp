#include <Drivers/Vulkan/VulkanGraphicsContext.h>
#include <Drivers/Vulkan/VulkanSwapchain.h>
#include <Drivers/Vulkan/VulkanDevice.h>
#include <Tabby/Core/Application.h>

#include <SDL_vulkan.h>
#include <SDL.h>

namespace Tabby {

VulkanSwapchain::VulkanSwapchain(const SwapchainSpecification& spec)
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

        // ImageSpecification swapchain_image_spec = {};
        // swapchain_image_spec.extent = { (uint32)m_Specification.extent.x, (uint32)m_Specification.extent.y, 1 };
        // swapchain_image_spec.usage = ImageUsage::RENDER_TARGET;
        // swapchain_image_spec.type = ImageType::TYPE_2D;
        // swapchain_image_spec.format = convert(m_SurfaceFormat.format);
        //
        // m_Images.push_back(std::make_shared<VulkanImage>(swapchain_image_spec, image, image_view));
        //
        // VkImageMemoryBarrier image_memory_barrier = {};
        // image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        // image_memory_barrier.image = m_SwapchainImages[i];
        // image_memory_barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        // image_memory_barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        // image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        // image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        // image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        // image_memory_barrier.subresourceRange.baseArrayLayer = 0;
        // image_memory_barrier.subresourceRange.layerCount = 1;
        // image_memory_barrier.subresourceRange.baseMipLevel = 0;
        // image_memory_barrier.subresourceRange.levelCount = 1;
        //
        // vkCmdPipelineBarrier(
        //     image_layout_transition_command_buffer,
        //     VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
        //     VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        //     0,
        //     0,
        //     nullptr,
        //     0,
        //     nullptr,
        //     1,
        //     &image_memory_barrier);
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
    for (auto& image_view : m_SwapchainImageViews) {
        vkDestroyImageView(device->Raw(), image_view, nullptr);
    }

    vkDestroySwapchainKHR(device->Raw(), m_Swapchain, nullptr);

    // for (auto& semaphores : m_Semaphores) {
    //     vkDestroySemaphore(device->Raw(), semaphores.render_complete, nullptr);
    //     vkDestroySemaphore(device->Raw(), semaphores.present_complete, nullptr);
    // }
    //
    // for (auto& fence : m_Fences) {
    //     vkDestroyFence(device->Raw(), fence, nullptr);
    // }

    m_Swapchain = VK_NULL_HANDLE;
}
}
