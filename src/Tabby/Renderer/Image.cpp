#include <Drivers/Vulkan/VulkanImage.h>
#include <Tabby/Renderer/Image.h>

namespace Tabby {

Shared<Image> Image::Create(const ImageSpecification& spec, const AssetHandle& id /* = UUID()*/)
{
    return std::make_shared<VulkanImage>(spec, id);
    return nullptr;
}

Shared<ImageSampler> ImageSampler::Create(const ImageSamplerSpecification& spec)
{
    return std::make_shared<VulkanImageSampler>(spec);
    return nullptr;
}

}
