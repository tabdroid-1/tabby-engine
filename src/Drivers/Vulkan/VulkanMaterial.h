#pragma once

#include <Drivers/Vulkan/VulkanShader.h>

#include <Tabby/Renderer/Material.h>

namespace Tabby {

class VulkanDescriptorSet;

class VulkanMaterial : public Material {
public:
    VulkanMaterial(const MaterialSpecification& spec, AssetHandle id);
    ~VulkanMaterial();

    void UploadData(const std::string& name, uint64_t offset, void* data, uint64_t data_size) override;
    void UploadData(const std::string& name, uint16_t array_element, Shared<Image> image, Shared<ImageSampler> sampler) override;

    void Destroy() override;

    std::vector<Shared<VulkanDescriptorSet>> GetDescriptorSets() { return m_DescriptorSets; }

private:
    struct VulkanUniformBufferData {
        uint16_t set;
        uint16_t binding;
        // null buffer implies this is a image uniform
        Shared<ShaderBuffer> buffer;

        // to be obviously used by image buffer. this is to prevent unnecessary write.
        // this is to allow calling UploadData function each frame.
        std::unordered_map<uint16_t, std::pair<Shared<Image>, Shared<ImageSampler>>> image_data;
    };

private:
    std::unordered_map<std::string, VulkanUniformBufferData> m_UniformBuffers;
    std::vector<Shared<VulkanDescriptorSet>> m_DescriptorSets;
};

}
