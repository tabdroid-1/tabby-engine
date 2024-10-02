#include <Drivers/Vulkan/VulkanMaterial.h>
#include <Tabby/Renderer/Material.h>

namespace Tabby {

Shared<Material> Material::Create(const MaterialSpecification& spec, AssetHandle handle)
{
    return CreateShared<VulkanMaterial>(spec, handle);
}

}
