#include <Drivers/Vulkan/VulkanShaderBuffer.h>

#include "ShaderBuffer.h"

namespace Tabby {

Shared<ShaderBuffer> ShaderBuffer::Create(const ShaderBufferSpecification& spec, Buffer data)
{
    return CreateShared<VulkanShaderBuffer>(spec, data);
}

}
