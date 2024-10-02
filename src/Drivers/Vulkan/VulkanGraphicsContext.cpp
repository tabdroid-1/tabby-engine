#include "VulkanGraphicsContext.h"
#include "VulkanMemoryAllocator.h"
#include "VulkanDebugUtil.h"
#include "VulkanSwapchain.h"
#include "VulkanRenderPass.h"
#include "VulkanDevice.h"

#include <Tabby/Renderer/ShaderLibrary.h>
#include <Tabby/Core/Application.h>

#include <SDL.h>
#include <SDL_vulkan.h>

namespace Tabby {

VulkanGraphicsContext::VulkanGraphicsContext()
{
    TB_CORE_ASSERT_TAGGED(!s_Instance, "Graphics context is already initialized.");

    s_Instance = this;

    VkApplicationInfo appInfo {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Tabby Engine App";
    appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
    appInfo.pEngineName = "Tabby Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
    appInfo.apiVersion = VK_API_VERSION_1_1;

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
#if defined(TB_PLATFORM_MACOS) || defined(TB_PLATFORM_ANDROID)
    extensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif
    createInfo.enabledExtensionCount = extensions.size();
    createInfo.ppEnabledExtensionNames = extensions.data();

    VK_CHECK_RESULT(vkCreateInstance(&createInfo, nullptr, &m_VulkanInstance));

#ifdef DEBUG
    m_DebugUtils = std::make_shared<VulkanDebugUtils>(this);
#endif

    VkPhysicalDeviceFeatures device_features = {};
    device_features.samplerAnisotropy = true;

    std::shared_ptr<VulkanPhysicalDevice> device = VulkanPhysicalDevice::Select(this);
    m_Device = std::make_shared<VulkanDevice>(device, std::forward<VkPhysicalDeviceFeatures>(device_features));

    VulkanMemoryAllocator::Init();

    m_Swapchain = std::make_shared<VulkanSwapchain>();
    m_Swapchain->CreateSurface();
    m_Swapchain->CreateSwapchain();

    m_RenderPass = std::make_shared<VulkanRenderPass>();
    m_RenderPass->CreateRenderPass();
    m_RenderPass->CreateFramebuffer();
}

VulkanGraphicsContext::~VulkanGraphicsContext() { Destroy(); }

void VulkanGraphicsContext::Destroy()
{
    vkDeviceWaitIdle(m_Device->Raw());
    m_RenderPass->DestroyFramebuffer();
    VulkanMemoryAllocator::Destroy();
    m_RenderPass->DestroyRenderPass();
    m_Swapchain->DestroySwapchain();
    m_Swapchain->DestroySurface();
    m_Device->Destroy();
#if TB_DEBUG
    m_DebugUtils->Destroy(this);
#endif // TB_DEBUG
    vkDestroyInstance(m_VulkanInstance, nullptr);
}

std::vector<const char*> VulkanGraphicsContext::GetVulkanExtensions()
{
    uint32_t extension_count = 0;
    const char** sdl_extensions;
    SDL_Vulkan_GetInstanceExtensions((SDL_Window*)Application::GetWindow().GetNativeWindow(), &extension_count, nullptr);
    sdl_extensions = new const char*[extension_count];
    SDL_Vulkan_GetInstanceExtensions((SDL_Window*)Application::GetWindow().GetNativeWindow(), &extension_count, sdl_extensions);

    std::vector<const char*> extensions(sdl_extensions,
        sdl_extensions + extension_count);

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
