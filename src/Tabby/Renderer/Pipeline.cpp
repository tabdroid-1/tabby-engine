#include <Drivers/Vulkan/VulkanPipeline.h>

namespace Tabby {

Shared<Pipeline> Pipeline::Create(const PipelineSpecification& spec)
{
    return CreateShared<VulkanPipeline>(spec);
}

}
