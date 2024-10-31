#pragma once

#include <Drivers/Vulkan/VulkanDescriptorSet.h>
#include <Drivers/Vulkan/VulkanCommon.h>

#include <Tabby/Renderer/ShaderBufferLayout.h>
#include <Tabby/Renderer/Shader.h>

namespace Tabby {

struct VulkanDescriptorInfo {
    std::vector<uint32_t> binding_sizes;
    std::vector<std::string> binding_names;
    std::vector<VulkanDescriptorBinding> bindings;
};

class VulkanShader : public Shader {
public:
    VulkanShader(std::map<ShaderStage, std::vector<uint32_t>> binaries, const std::string& path);
    ~VulkanShader();

    const ShaderBufferLayout& GetBufferLayout() { return m_BufferLayout; };
    std::vector<VulkanDescriptorInfo> GetDescriptorInfos() const { return m_DescriptorInfos; }
    std::vector<VkPipelineShaderStageCreateInfo> GetCreateInfos() const { return m_StageCreateInfos; }
    std::vector<VkDescriptorSetLayout> GetLayouts() const { return m_SetLayouts; }
    std::vector<VkPushConstantRange> GetRanges() const { return m_Ranges; }
    void Destroy() override;

private:
    ShaderBufferLayout m_BufferLayout;
    std::vector<VulkanDescriptorInfo> m_DescriptorInfos;

    std::vector<VkPipelineShaderStageCreateInfo> m_StageCreateInfos;
    std::vector<VkDescriptorSetLayout> m_SetLayouts;
    std::vector<VkPushConstantRange> m_Ranges;
    bool m_Dirty = false;
};

}
