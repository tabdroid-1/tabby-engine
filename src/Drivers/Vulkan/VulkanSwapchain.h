#pragma once

#include "VulkanCommon.h"

namespace Tabby {

class VulkanDevice;
class Window;

struct SwapchainSpecification {
    Window* main_window;
    int width;
    int height;
    int frames_in_flight;
    bool vsync;
    bool use_depth;
};

class VulkanSwapchain {
public:
    struct SwapchainSemaphores {
        VkSemaphore render_complete;
        VkSemaphore present_complete;
    };

    VulkanSwapchain();
    ~VulkanSwapchain();

    void CreateSurface();
    void CreateSwapchain();
    void DestroySwapchain();
    void DestroySurface();

    void WaitFence();
    void BeginFrame();
    void EndFrame();

    // VkSurfaceKHR RawSurface() const { return m_Surface; }
    VkFormat RawImageFormat() const { return m_ImageFormat; }
    VkSurfaceFormatKHR RawSurfaceFormat() const { return m_SurfaceFormat; }
    VkSwapchainKHR RawSwapchain() const { return m_Swapchain; }
    std::vector<VkImageView> RawImageViews() const { return m_ImageViews; }
    VkExtent2D RawExtend() const { return m_Extent; }

    bool IsVSync() const { return m_Specification.vsync; };
    void SetVSync(bool vsync);

    SwapchainSpecification GetSpecification() { return m_Specification; }
    SwapchainSemaphores GetSemaphores() const { return m_Semaphores[m_CurrentFrameIndex]; }
    VkFence GetCurrentFence() const { return m_Fences[m_CurrentFrameIndex]; }
    VkImage GetCurrentImage() { return m_Images[m_CurrentImageIndex]; };

    uint32_t GetCurrentFrameIndex() const { return m_CurrentFrameIndex; }
    uint32_t GetCurrentImageIndex() { return m_CurrentImageIndex; };

private:
    SwapchainSpecification m_Specification;
    VkSwapchainKHR m_Swapchain;
    VkSurfaceKHR m_Surface;

    VkPresentModeKHR m_CurrentPresentMode;
    VkSurfaceFormatKHR m_SurfaceFormat;
    std::vector<VkImage> m_Images;
    VkFormat m_ImageFormat;
    VkExtent2D m_Extent;
    bool m_SupportsMailboxPresentation;
    std::vector<VkImageView> m_ImageViews;
    VkSurfaceCapabilitiesKHR m_SurfaceCapabilities;

    std::vector<SwapchainSemaphores> m_Semaphores;
    std::vector<VkFence> m_Fences;

    uint32_t m_CurrentFrameIndex = 0;
    uint32_t m_CurrentImageIndex = 0;
};

}
