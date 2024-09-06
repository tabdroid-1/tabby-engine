#pragma once

#include <tbpch.h>
#include <Tabby/Foundation/Types.h>

#include <volk.h>

namespace Tabby {

#ifdef TB_DEBUG
#define VK_CHECK_RESULT(fn)                                                                   \
    {                                                                                         \
        VkResult res = fn;                                                                    \
        if (res != VK_SUCCESS) {                                                              \
            std::string message = "Vulkan call failed (" + Util::VkResultToString(res) + ")"; \
            TB_CORE_ASSERT_TAGGED(false, message);                                            \
        }                                                                                     \
    }
#else
#define VK_CHECK_RESULT(fn) fn
#endif
namespace Util {
    const std::string VkResultToString(VkResult result);

}

#if defined(__ANDROID__)
#define VK_USE_PLATFORM_ANDROID_KHR
#endif

#if defined(__APPLE__)
#define VK_USE_PLATFORM_IOS_MVK
#define VK_USE_PLATFORM_MACOS_MVK
#define VK_USE_PLATFORM_METAL_EXT
#endif

#if defined(WIN32)
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#if defined(__linux__)
#define VK_USE_PLATFORM_WAYLAND_KHR
#define VK_USE_PLATFORM_XCB_KHR
#define VK_USE_PLATFORM_XLIB_KHR
#define VK_USE_PLATFORM_XLIB_XRANDR_EXT
#endif

}
