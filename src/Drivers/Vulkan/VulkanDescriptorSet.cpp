#include <Drivers/Vulkan/VulkanGraphicsContext.h>
#include <Drivers/Vulkan/VulkanDescriptorSet.h>
#include <Drivers/Vulkan/VulkanShaderBuffer.h>
#include <Drivers/Vulkan/VulkanRendererAPI.h>
#include <Drivers/Vulkan/VulkanDevice.h>
#include <Tabby/Renderer/ShaderBuffer.h>
#include <vulkan/vulkan_core.h>

namespace Tabby {

#pragma region converts
constexpr VkDescriptorType UsageToDescriptorType(const ShaderBufferUsage& usage)
{
    switch (usage) {
    case ShaderBufferUsage::UNIFORM_BUFFER:
        return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    case ShaderBufferUsage::STORAGE_BUFFER:
        return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    default:
        TB_CORE_ASSERT(false);
        return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    }
}

constexpr VkDescriptorType convert(const VulkanDescriptorBindingType& type)
{
    switch (type) {
    case VulkanDescriptorBindingType::SAMPLED_IMAGE:
        return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    case VulkanDescriptorBindingType::STORAGE_IMAGE:
        return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    case VulkanDescriptorBindingType::UNIFORM_BUFFER:
        return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    case VulkanDescriptorBindingType::STORAGE_BUFFER:
        return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    default:
        TB_CORE_ASSERT(false);
        return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    }
}
#pragma endregion

constexpr VkDescriptorBindingFlags extractFlags(const BitMask& mask)
{
    VkDescriptorBindingFlags flags = 0;

    if (mask & (uint32_t)VulkanDescriptorFlags::PARTIALLY_BOUND)
        flags |= VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT;

    return flags;
}

VulkanDescriptorSet::VulkanDescriptorSet(const VulkanDescriptorSetSpecification& spec)
    : m_DescriptorSet(VK_NULL_HANDLE)
    , m_Layout(VK_NULL_HANDLE)
{
    auto device = VulkanGraphicsContext::Get()->GetDevice();

    std::vector<VkDescriptorSetLayoutBinding> bindings;
    std::vector<VkDescriptorBindingFlags> binding_flags;

    for (auto& binding : spec.bindings) {
        VkDescriptorSetLayoutBinding vk_binding = {};
        vk_binding.stageFlags = VK_SHADER_STAGE_ALL;
        vk_binding.pImmutableSamplers = nullptr;
        vk_binding.descriptorCount = binding.array_count;
        vk_binding.descriptorType = convert(binding.type);
        vk_binding.binding = binding.binding;

        bindings.push_back(vk_binding);
        binding_flags.push_back(extractFlags(binding.flags));
    }

    // VkDescriptorSetLayoutBindingFlagsCreateInfo vk_binding_flags = {};
    // vk_binding_flags.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
    // vk_binding_flags.bindingCount = spec.bindings.size();
    // vk_binding_flags.pBindingFlags = binding_flags.data();

    VkDescriptorSetLayoutCreateInfo layout_create_info = {};
    layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layout_create_info.pNext = nullptr;
    layout_create_info.bindingCount = bindings.size();
    layout_create_info.pBindings = bindings.data();
    layout_create_info.flags = 0;

    VK_CHECK_RESULT(vkCreateDescriptorSetLayout(device->Raw(), &layout_create_info, nullptr, &m_Layout));

    std::vector<VkDescriptorSet> set = VulkanRendererAPI::AllocateDescriptorSets(m_Layout, 1);
    m_DescriptorSet = set[0];
}

VulkanDescriptorSet::VulkanDescriptorSet(VkDescriptorSetLayout layer)
    : m_DescriptorSet(VK_NULL_HANDLE)
    , m_Layout(layer)
{
    auto device = VulkanGraphicsContext::Get()->GetDevice();

    std::vector<VkDescriptorSet> set = VulkanRendererAPI::AllocateDescriptorSets(m_Layout, 1);
    m_DescriptorSet = set[0];
}

VulkanDescriptorSet::~VulkanDescriptorSet()
{
    auto device = VulkanGraphicsContext::Get()->GetDevice();
    vkDestroyDescriptorSetLayout(device->Raw(), m_Layout, nullptr);
    VulkanRendererAPI::FreeDescriptorSets({ m_DescriptorSet });
}

void VulkanDescriptorSet::Destroy()
{
    auto device = VulkanGraphicsContext::Get()->GetDevice();
    VulkanRendererAPI::FreeDescriptorSets({ m_DescriptorSet });
    if (m_Layout != VK_NULL_HANDLE)
        vkDestroyDescriptorSetLayout(device->Raw(), m_Layout, nullptr);
}

// void VulkanDescriptorSet::Write(uint16 binding, uint16 array_element, Shared<Image> image, Shared<ImageSampler> sampler)
// {
//     auto device = VulkanGraphicsContext::Get()->GetDevice();
//     Shared<VulkanImage> vk_image = ShareAs<VulkanImage>(image);
//     Shared<VulkanImageSampler> vk_sampler = ShareAs<VulkanImageSampler>(sampler);
//
//     VkDescriptorImageInfo descriptor_image_info = {};
//     descriptor_image_info.imageView = vk_image->RawView();
//     descriptor_image_info.sampler = vk_sampler->Raw();
//     descriptor_image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
//
//     VkWriteDescriptorSet write_descriptor_set = {};
//     write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//     write_descriptor_set.dstSet = m_DescriptorSet;
//     write_descriptor_set.dstBinding = binding;
//     write_descriptor_set.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
//     write_descriptor_set.dstArrayElement = array_element;
//     write_descriptor_set.descriptorCount = 1;
//     write_descriptor_set.pImageInfo = &descriptor_image_info;
//
//     vkUpdateDescriptorSets(device->Raw(), 1, &write_descriptor_set, 0, nullptr);
// }

void VulkanDescriptorSet::Write(uint16_t binding, uint16_t array_element, ShaderBuffer* buffer, uint64_t size, uint64_t offset)
{
    auto device = VulkanGraphicsContext::Get()->GetDevice();
    VulkanShaderBuffer* vk_buffer = (VulkanShaderBuffer*)buffer;

    VkDescriptorBufferInfo descriptor_buffer_info = {};
    descriptor_buffer_info.buffer = vk_buffer->Raw();
    descriptor_buffer_info.range = size;
    descriptor_buffer_info.offset = offset;

    VkWriteDescriptorSet write_descriptor_set = {};
    write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write_descriptor_set.dstSet = m_DescriptorSet;
    write_descriptor_set.dstBinding = binding;
    write_descriptor_set.descriptorType = UsageToDescriptorType(vk_buffer->GetSpecification().buffer_usage);
    write_descriptor_set.dstArrayElement = array_element;
    write_descriptor_set.descriptorCount = 1;
    write_descriptor_set.pBufferInfo = &descriptor_buffer_info;

    vkUpdateDescriptorSets(device->Raw(), 1, &write_descriptor_set, 0, nullptr);
}

void VulkanDescriptorSet::Write(uint16_t binding, uint16_t array_element, Shared<ShaderBuffer> buffer, uint64_t size, uint64_t offset)
{
    auto device = VulkanGraphicsContext::Get()->GetDevice();
    Shared<VulkanShaderBuffer> vk_buffer = ShareAs<VulkanShaderBuffer>(buffer);

    VkDescriptorBufferInfo descriptor_buffer_info = {};
    descriptor_buffer_info.buffer = vk_buffer->Raw();
    descriptor_buffer_info.range = size;
    descriptor_buffer_info.offset = offset;

    VkWriteDescriptorSet write_descriptor_set = {};
    write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write_descriptor_set.dstSet = m_DescriptorSet;
    write_descriptor_set.dstBinding = binding;
    write_descriptor_set.descriptorType = UsageToDescriptorType(vk_buffer->GetSpecification().buffer_usage);
    write_descriptor_set.dstArrayElement = array_element;
    write_descriptor_set.descriptorCount = 1;
    write_descriptor_set.pBufferInfo = &descriptor_buffer_info;

    vkUpdateDescriptorSets(device->Raw(), 1, &write_descriptor_set, 0, nullptr);
}

}
