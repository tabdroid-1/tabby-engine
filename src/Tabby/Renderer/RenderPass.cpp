#include <Drivers/Vulkan/VulkanRenderPass.h>

namespace Tabby {

Shared<RenderPass> RenderPass::Create(const RenderPassSpecification& spec)
{
    return CreateShared<VulkanRenderPass>(spec);
}

}
