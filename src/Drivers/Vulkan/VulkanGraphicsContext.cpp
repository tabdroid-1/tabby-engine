#define VOLK_IMPLEMENTATION
#include <volk.h>

#include <Drivers/Vulkan/VulkanGraphicsContext.h>
#include <Drivers/Vulkan/VulkanDebugUtils.h>
#include <Drivers/Vulkan/VulkanSwapchain.h>
#include <Drivers/Vulkan/VulkanDevice.h>
#include <Drivers/Vulkan/VulkanCommon.h>
#include <Tabby/Foundation/Types.h>
#include <Tabby/Core/Application.h>

#include <vk_mem_alloc.h>
#include <SDL_vulkan.h>
#include <SDL.h>

namespace Tabby {

VulkanGraphicsContext::VulkanGraphicsContext(const RendererConfig& config)
{
    if (s_Instance) {
        TB_CORE_ERROR("Graphics context is already initialized.");
        return;
    }

    s_Instance = this;

    volkInitialize();

    VkApplicationInfo appInfo {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Tabby Engine App";
    appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
    appInfo.pEngineName = "Tabby Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    std::vector<const char*> extensions = GetVulkanExtensions();
    std::vector<const char*> layers = GetVulkanLayers();

    VkInstanceCreateInfo createInfo {};

#if TB_DEBUG
    VkDebugUtilsMessengerCreateInfoEXT messenger_create_info = VulkanDebugUtils::GetMessengerCreateInfo();
    createInfo.pNext = &messenger_create_info;
#endif
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledLayerCount = layers.size();
    createInfo.ppEnabledLayerNames = layers.data();
#ifdef TB_PLATFORM_MACOS
    extensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif
    createInfo.enabledExtensionCount = extensions.size();
    createInfo.ppEnabledExtensionNames = extensions.data();

    VK_CHECK_RESULT(vkCreateInstance(&createInfo, nullptr, &m_VulkanInstance));
    volkLoadInstance(m_VulkanInstance);

#if TB_DEBUG
    m_DebugUtils = std::make_shared<VulkanDebugUtils>(this);
#endif

    VkPhysicalDeviceFeatures device_features = {};
    device_features.samplerAnisotropy = true;
    device_features.geometryShader = true;
    device_features.tessellationShader = true;
    device_features.shaderInt64 = true;
    device_features.wideLines = true;
    device_features.shaderInt16 = true;

    Shared<VulkanPhysicalDevice> device = VulkanPhysicalDevice::Select(this);
    m_Device = std::make_shared<VulkanDevice>(device, std::forward<VkPhysicalDeviceFeatures>(device_features));
    volkLoadDevice(m_Device->Raw());

    SDL_Window* window_handle = (SDL_Window*)config.main_window;
    IntVector2 window_size = {};
    SDL_GetWindowSize(window_handle, &window_size.x, &window_size.y);

    SwapchainSpecification swapchain_spec = {};
    swapchain_spec.main_window = config.main_window;
    swapchain_spec.frames_in_flight = config.frames_in_flight;
    swapchain_spec.extent = window_size;
    swapchain_spec.vsync = config.vsync;

    // m_Swapchain = std::make_shared<VulkanSwapchain>(swapchain_spec);
    // m_Swapchain->CreateSurface(swapchain_spec);
    // m_Swapchain->CreateSwapchain(swapchain_spec);

    // VulkanMemoryAllocator::Init();
}

VulkanGraphicsContext::~VulkanGraphicsContext()
{
    Destroy();
}

void VulkanGraphicsContext::Destroy()
{
    vkDeviceWaitIdle(m_Device->Raw());
    // VulkanMemoryAllocator::Destroy();
    // m_Swapchain->DestroySwapchain();
    // m_Swapchain->DestroySurface();
    m_Device->Destroy();
#if TB_DEBUG
    m_DebugUtils->Destroy(this);
#endif // TB_DEBUG
    vkDestroyInstance(m_VulkanInstance, nullptr);
    volkFinalize();
}

std::vector<const char*> VulkanGraphicsContext::GetVulkanExtensions()
{
    uint32_t extension_count = 0;
    const char** sdl_extensions;
    SDL_Vulkan_GetInstanceExtensions((SDL_Window*)Application::GetWindow().GetNativeWindow(), &extension_count, nullptr);
    sdl_extensions = new const char*[extension_count];
    SDL_Vulkan_GetInstanceExtensions((SDL_Window*)Application::GetWindow().GetNativeWindow(), &extension_count, sdl_extensions);

    std::vector<const char*> extensions(sdl_extensions, sdl_extensions + extension_count);

#ifdef TB_DEBUG
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif // TB_DEBUG

    extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

    return extensions;
}

std::vector<const char*> VulkanGraphicsContext::GetVulkanLayers()
{
    uint32_t property_count = 0;
    vkEnumerateInstanceLayerProperties(&property_count, nullptr);

    std::vector<VkLayerProperties> layer_properties(property_count);
    vkEnumerateInstanceLayerProperties(&property_count, layer_properties.data());

    std::vector<const char*> layers;
#ifdef TB_DEBUG
    layers.push_back("VK_LAYER_KHRONOS_validation");
#endif // TB_DEBUG

    return layers;
}
}
