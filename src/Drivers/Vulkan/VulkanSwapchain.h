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

    // void BeginFrame();
    // void EndFrame();
    //
    // VkSurfaceKHR RawSurface() const { return m_Surface; }
    // VkSwapchainKHR RawSwapchain() const { return m_Swapchain; }
    //
    // bool IsVSync() const { return m_Specification.vsync; };
    // void SetVSync(bool vsync);
    //
    // SwapchainSpecification GetSpecification() { return m_Specification; }
    // uint32_t GetCurrentFrameIndex() const { return m_CurrentFrameIndex; }
    //
    // SwapchainSemaphores GetSemaphores() const { return m_Semaphores[m_CurrentFrameIndex]; }
    // VkFence GetCurrentFence() const { return m_Fences[m_CurrentFrameIndex]; }
    // Shared<Image> GetCurrentImage() { return ShareAs<Image>(m_Images[m_CurrentImageIndex]); };

private:
    SwapchainSpecification m_Specification;
    VkSwapchainKHR m_Swapchain;
    VkFormat m_SwapchainImageFormat;

    std::vector<VkImage> m_SwapchainImages;
    std::vector<VkImageView> m_SwapchainImageViews;
    VkExtent2D m_SwapchainExtent;
    VkSurfaceKHR m_Surface;
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
    // uint32 m_CurrentFrameIndex = 0;
    // uint32 m_CurrentImageIndex = 0;
};

}
