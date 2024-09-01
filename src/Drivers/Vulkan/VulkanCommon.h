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

}
