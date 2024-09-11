#include "GraphicsContext.h"

#include <Drivers/Vulkan/VulkanGraphicsContext.h>

namespace Tabby {

Shared<GraphicsContext> GraphicsContext::Create()
{
    return std::make_shared<VulkanGraphicsContext>();
}

}
