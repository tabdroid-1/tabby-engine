#include <Drivers/Vulkan/VulkanShaderBuffer.h>

#include "ShaderBuffer.h"

namespace Tabby {

Shared<ShaderBuffer> ShaderBuffer::Create(const ShaderBufferSpecification& spec, Buffer data)
{
    return CreateShared<VulkanShaderBuffer>(spec, data);
}

Shared<ShaderBuffer> ShaderBuffer::Create(const ShaderBufferSpecification& spec, void* data, uint64_t data_size)
{
    return CreateShared<VulkanShaderBuffer>(spec, data, data_size);
}

}
