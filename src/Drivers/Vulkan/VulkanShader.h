#pragma once

#include <Drivers/Vulkan/VulkanDescriptorSet.h>
#include <Drivers/Vulkan/VulkanCommon.h>

#include <Tabby/Renderer/Shader.h>

namespace Tabby {

class VulkanPipeline;

struct VulkanDescriptorInfo {
    std::vector<uint32_t> binding_sizes;
    std::vector<std::string> binding_names;
    std::vector<VulkanDescriptorBinding> bindings;
};

class VulkanShader : public Shader {
public:
    VulkanShader(const ShaderSpecification& spec, std::map<ShaderStage, std::vector<uint32_t>> binaries);
    ~VulkanShader();

    Shared<VulkanPipeline> GetPipeline() { return m_Pipeline; }
    std::vector<VulkanDescriptorInfo> GetDescriptorInfos() const { return m_DescriptorInfos; }
    std::vector<VkPipelineShaderStageCreateInfo> GetCreateInfos() const { return m_StageCreateInfos; }
    std::vector<VkDescriptorSetLayout> GetLayouts() const { return m_SetLayouts; }
    std::vector<VkPushConstantRange> GetRanges() const { return m_Ranges; }
    void Destroy() override;

private:
    ShaderSpecification m_Specification;

    Shared<VulkanPipeline> m_Pipeline;

    std::vector<ShaderDataType> m_VertexInputLayout;
    std::vector<VulkanDescriptorInfo> m_DescriptorInfos;

    std::vector<VkPipelineShaderStageCreateInfo> m_StageCreateInfos;
    std::vector<VkDescriptorSetLayout> m_SetLayouts;
    std::vector<VkPushConstantRange> m_Ranges;
    bool m_Dirty = false;
};

}
