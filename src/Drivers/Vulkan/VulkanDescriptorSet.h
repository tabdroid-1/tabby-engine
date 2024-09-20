#pragma once

#include <Tabby/Renderer/ShaderBuffer.h>

#include <Drivers/Vulkan/VulkanCommon.h>

namespace Tabby {

enum class VulkanDescriptorBindingRate {
    SCENE = 0,
    PASS = 1,
    DRAW_CALL = 2
};

enum class VulkanDescriptorBindingType : uint32_t {
    SAMPLED_IMAGE,
    STORAGE_IMAGE,
    UNIFORM_BUFFER,
    STORAGE_BUFFER
};

enum class VulkanDescriptorFlags : uint32_t {
    PARTIALLY_BOUND = BIT(0)
};

struct VulkanDescriptorBinding {
    uint32_t binding;
    VulkanDescriptorBindingType type;
    uint32_t array_count;
    BitMask flags;
};

struct VulkanDescriptorSetSpecification {
    std::vector<VulkanDescriptorBinding> bindings;
};

class VulkanDescriptorSet {
public:
    VulkanDescriptorSet(const VulkanDescriptorSetSpecification& spec);
    VulkanDescriptorSet(VkDescriptorSetLayout layout);
    ~VulkanDescriptorSet();

    void Destroy();

    VkDescriptorSet Raw() const { return m_DescriptorSet; }
    VkDescriptorSetLayout RawLayout() const { return m_Layout; }

    // void Write(uint16 binding, uint16 array_element, Shared<Image> image, Shared<ImageSampler> sampler) override;
    void Write(uint16_t binding, uint16_t array_element, ShaderBuffer* buffer, uint64_t size, uint64_t offset);
    void Write(uint16_t binding, uint16_t array_element, Shared<ShaderBuffer> buffer, uint64_t size, uint64_t offset);

private:
    VkDescriptorSet m_DescriptorSet;
    VkDescriptorSetLayout m_Layout;
};

}
