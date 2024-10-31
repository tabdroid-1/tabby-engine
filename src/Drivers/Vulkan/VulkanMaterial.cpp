#include <Drivers/Vulkan/VulkanDescriptorSet.h>
#include <Drivers/Vulkan/VulkanShaderBuffer.h>
#include <Drivers/Vulkan/VulkanMaterial.h>
#include <Drivers/Vulkan/VulkanShader.h>

namespace Tabby {

VulkanMaterial::VulkanMaterial(const MaterialSpecification& spec, AssetHandle id)
    : Material(id, spec)
{
    if (m_DescriptorSets.size()) {
        for (auto& set : m_DescriptorSets)
            set->Destroy();
    }
    m_DescriptorSets.clear();

    auto vk_shader = ShareAs<VulkanShader>(m_Specification.pipeline->GetSpecification().shader);

    auto descriptorset_infos = vk_shader->GetDescriptorInfos();

    // Create Descriptor sets
    m_DescriptorSets.resize(descriptorset_infos.size());
    for (int i = 0; i < descriptorset_infos.size(); i++) {
        VulkanDescriptorSetSpecification descriptorset_spec;
        descriptorset_spec.bindings = vk_shader->GetDescriptorInfos()[i].bindings;
        auto descriptor = CreateShared<VulkanDescriptorSet>(descriptorset_spec);
        m_DescriptorSets[i] = descriptor;
    }

    // Create uniform buffers
    for (int i = 0; i < descriptorset_infos.size(); i++) {

        for (int k = 0; k < descriptorset_infos[i].bindings.size(); k++) {

            if (descriptorset_infos[i].bindings[k].type == VulkanDescriptorBindingType::UNIFORM_BUFFER || descriptorset_infos[i].bindings[k].type == VulkanDescriptorBindingType::STORAGE_BUFFER) {

                ShaderBufferSpecification buffer_spec = {};
                buffer_spec.buffer_usage = ShaderBufferUsage::UNIFORM_BUFFER;
                buffer_spec.heap = ShaderBufferMemoryHeap::HOST;
                buffer_spec.memory_usage = ShaderBufferMemoryUsage::COHERENT_WRITE;
                buffer_spec.size = descriptorset_infos[i].binding_sizes[k];

                auto buffer = ShareAs<VulkanShaderBuffer>(ShaderBuffer::Create(buffer_spec, malloc(descriptorset_infos[i].binding_sizes[k]), descriptorset_infos[i].binding_sizes[k]));

                VulkanUniformBufferData buffer_data;
                buffer_data.set = i;
                buffer_data.binding = k;
                buffer_data.buffer = buffer;
                m_UniformBuffers[descriptorset_infos[i].binding_names[k]] = buffer_data;
                m_DescriptorSets[i]->Write(buffer_data.binding, 0, m_UniformBuffers[descriptorset_infos[i].binding_names[k]].buffer, descriptorset_infos[i].binding_sizes[k], 0);
            } else {
                VulkanUniformBufferData buffer_data;
                buffer_data.set = i;
                buffer_data.binding = k;
                buffer_data.buffer = nullptr;
                m_UniformBuffers[descriptorset_infos[i].binding_names[k]] = buffer_data;
            }
        }
    }
}

VulkanMaterial::~VulkanMaterial()
{
}

void VulkanMaterial::Destroy()
{
    std::vector<std::pair<VulkanDescriptorInfo, Shared<VulkanDescriptorSet>>> m_DescriptorSets;

    for (auto& buffer : m_UniformBuffers) {
        if (buffer.second.buffer) // image uniform buffers doesnt have designated shaderbuffer
            buffer.second.buffer->Destroy();
    }

    for (auto& descriptor : m_DescriptorSets) {
        descriptor.second->Destroy();
    }
}

void VulkanMaterial::UploadData(const std::string& name, uint64_t offset, void* data, uint64_t data_size)
{
    if (m_UniformBuffers.find(name) == m_UniformBuffers.end()) {
        TB_CORE_WARN("No buffer with name \"{}\" found.", name);
        return;
    }

    if (m_UniformBuffers[name].buffer == nullptr) {
        TB_CORE_WARN("\"{}\" is image buffer. Only image can be uploaded!", name);
        return;
    }

    m_UniformBuffers[name].buffer->UploadData(offset, data, data_size);
}

void VulkanMaterial::UploadData(const std::string& name, uint16_t array_element, Shared<Image> image, Shared<ImageSampler> sampler)
{
    if (m_UniformBuffers.find(name) == m_UniformBuffers.end()) {
        TB_CORE_WARN("No buffer with name \"{}\" found.", name);
        return;
    }

    if (m_UniformBuffers[name].buffer != nullptr) {
        TB_CORE_WARN("\"{}\" is uniform buffer. Only data other than image can be uploaded!", name);
        return;
    }

    if (m_UniformBuffers[name].image_data[array_element].first != image || m_UniformBuffers[name].image_data[array_element].second != sampler) {

        m_UniformBuffers[name].image_data[array_element].first = image;
        m_UniformBuffers[name].image_data[array_element].second = sampler;

        m_DescriptorSets[m_UniformBuffers[name].set]->Write(m_UniformBuffers[name].binding, array_element, image, sampler);
    }
}

}
