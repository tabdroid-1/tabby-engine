#pragma once

#include "VulkanCommon.h"

namespace Tabby {

class VulkanDevice;
class VulkanImage;
class Window;

struct SwapchainSpecification {
    Window* main_window;
    UIntVector2 extent;
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

    VulkanSwapchain(const SwapchainSpecification& spec);
    ~VulkanSwapchain();

    void CreateSurface();
    void CreateSwapchain();
    void DestroySwapchain();
    void DestroySurface();

    void WaitFence();
    void BeginFrame();
    void EndFrame();

    VkSurfaceFormatKHR RawSurfaceFormat() const { return m_SurfaceFormat; }
    VkSwapchainKHR RawSwapchain() const { return m_Swapchain; }

    bool IsVSync() const { return m_Specification.vsync; };
    void SetVSync(bool vsync);

    SwapchainSpecification GetSpecification() { return m_Specification; }
    SwapchainSemaphores GetSemaphores() const { return m_Semaphores[m_CurrentFrameIndex]; }
    VkFence GetCurrentFence() const { return m_Fences[m_CurrentFrameIndex]; }
    Shared<VulkanImage> GetCurrentImage() { return m_Images[m_CurrentImageIndex]; };
    std::vector<Shared<VulkanImage>> GetImages() { return m_Images; };

    uint32_t GetCurrentFrameIndex() const { return m_CurrentFrameIndex; }
    uint32_t GetCurrentImageIndex() { return m_CurrentImageIndex; };

private:
    SwapchainSpecification m_Specification;
    VkSwapchainKHR m_Swapchain;
    VkSurfaceKHR m_Surface;

    VkSurfaceFormatKHR m_SurfaceFormat;
    VkPresentModeKHR m_CurrentPresentMode;
    bool m_SupportsMailboxPresentation;

    std::vector<Shared<VulkanImage>> m_Images;

    std::vector<SwapchainSemaphores> m_Semaphores;
    std::vector<VkFence> m_Fences;

    const uint8_t m_SwachainImageCount = 3;

    uint32_t m_CurrentFrameIndex = 0;
    uint32_t m_CurrentImageIndex = 0;
};

}
