#pragma once

#include <Drivers/Vulkan/VulkanCommon.h>

namespace Tabby {

class VulkanDevice;
class Window;

struct SwapchainSpecification {
    Window* main_window;
    IntVector2 extent;
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

    void CreateSurface(const SwapchainSpecification& spec);
    void CreateSwapchain(const SwapchainSpecification& spec);
    void CreateSwapchain();

    void DestroySurface();
    void DestroySwapchain();

    void WaitFence();
    void BeginFrame();
    void EndFrame();

    VkSurfaceKHR RawSurface() const { return m_Surface; }
    VkSurfaceFormatKHR RawSurfaceFormat() const { return m_SurfaceFormat; }
    VkSwapchainKHR RawSwapchain() const { return m_Swapchain; }
    std::vector<VkImageView> RawImageViews() const { return m_SwapchainImageViews; }

    bool IsVSync() const { return m_Specification.vsync; };
    void SetVSync(bool vsync);

    SwapchainSpecification GetSpecification() { return m_Specification; }
    SwapchainSemaphores GetSemaphores() const { return m_Semaphore; }
    VkFence GetCurrentFence() const { return m_Fence; }

    // uint32_t GetCurrentFrameIndex() const { return m_CurrentFrameIndex; }

    // SwapchainSemaphores GetSemaphores() const { return m_Semaphores[m_CurrentFrameIndex]; }
    // VkFence GetCurrentFence() const { return m_Fences[m_CurrentFrameIndex]; }
    // Shared<Image> GetCurrentImage() { return ShareAs<Image>(m_Images[m_CurrentImageIndex]); };

private:
    SwapchainSpecification m_Specification;
    VkSurfaceKHR m_Surface;
    VkSwapchainKHR m_Swapchain;

    VkSurfaceCapabilitiesKHR m_SurfaceCapabilities;
    VkPresentModeKHR m_CurrentPresentMode;
    VkSurfaceFormatKHR m_SurfaceFormat;
    std::vector<VkImage> m_SwapchainImages;
    std::vector<VkImageView> m_SwapchainImageViews;
    bool m_SupportsMailboxPresentation;
    const uint32_t m_SwachainImageCount = 3;

    SwapchainSemaphores m_Semaphore;
    VkFence m_Fence;

    // std::vector<SwapchainSemaphores> m_Semaphores;
    // std::vector<VkFence> m_Fences;

    // SwapchainSpecification m_Specification;
    // VkSwapchainKHR m_Swapchain;
    // VkFormat m_SwapchainImageFormat;
    //
    // std::vector<VkImage> m_SwapchainImages;
    // std::vector<VkImageView> m_SwapchainImageViews;
    // VkExtent2D m_SwapchainExtent;
    // VkSurfaceKHR m_Surface;
    // VkSwapchainKHR m_Swapchain;
    //
    // VkSurfaceFormatKHR m_SurfaceFormat;
    // VkPresentModeKHR m_CurrentPresentMode;
    // bool m_SupportsMailboxPresentation;
    //
    // std::vector<Shared<VulkanImage>> m_Images;
    //
    // std::vector<SwapchainSemaphores> m_Semaphores;
    // std::vector<VkFence> m_Fences;
    //
    // const uint32 m_SwachainImageCount = 3;
    //
    uint32_t m_CurrentFrameIndex = 0;
    uint32_t m_CurrentImageIndex = 0;
};

}
