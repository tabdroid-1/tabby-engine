#pragma once

#include <Tabby/Renderer/Shader.h>

#include "VulkanCommon.h"

namespace Tabby {

class VulkanShader : public Shader {
public:
    VulkanShader(const ShaderSpecification& spec, std::map<ShaderStage, std::vector<uint32_t>> binaries);
    ~VulkanShader();

    VkPipeline RawPipeline() const { return m_Pipeline; }
    std::vector<VkPipelineShaderStageCreateInfo> GetCreateInfos() const { return m_StageCreateInfos; }
    std::vector<VkDescriptorSetLayout> GetLayouts() const { return m_SetLayouts; }
    std::vector<VkPushConstantRange> GetRanges() const { return m_Ranges; }
    void Destroy() override;

    /*
     *	@brief returns status of "dirty" bit. It indicates if shader is still valid. It may be invalid due to several reasons:
     *	unsuccessful reflection, reload required etc.
     */

    // void RestoreShaderModule(std::filesystem::path path) override;

private:
    void CreateGraphicsPipeline();
    void CreateComputePipeline();

private:
    ShaderSpecification m_Specification;

    std::vector<VkPipelineShaderStageCreateInfo> m_StageCreateInfos;
    std::vector<VkDescriptorSetLayout> m_SetLayouts;
    std::vector<VkPushConstantRange> m_Ranges;
    VkPipelineLayout m_PipelineLayout;
    VkPipeline m_Pipeline;
    bool m_Dirty = false;
};

}

// #pragma once
//
// #include "VulkanCommon.h"
//
// #include <Tabby/Renderer/Shader.h>
//
// namespace Tabby {
//
// class VulkanPipeline;
//
// class VulkanShader : public Shader {
// public:
//     VulkanShader(std::map<ShaderStage, std::vector<uint8_t>> binaries);
//     ~VulkanShader();
//
//     std::vector<VkPipelineShaderStageCreateInfo> GetCreateInfos() const { return m_StageCreateInfos; }
//     std::vector<VkDescriptorSetLayout> GetLayouts() const { return m_SetLayouts; }
//     std::vector<VkPushConstantRange> GetRanges() const { return m_Ranges; }
//
//     VkPipeline RawPipeline() const { return m_Pipeline; }
//
//     void Destroy();
//
//     /*
//      *	@brief returns status of "dirty" bit. It indicates if shader is still valid. It may be invalid due to several reasons:
//      *	unsuccessful reflection, reload required etc.
//      */
//
//     void RestoreShaderModule(const std::string& path);
//
// private:
//     // std::shared_ptr<VulkanPipeline> m_Pipeline;
//
//     VkPipelineLayout m_PipelineLayout;
//     VkPipeline m_Pipeline;
//
//     std::vector<VkPipelineShaderStageCreateInfo> m_StageCreateInfos;
//     std::vector<VkDescriptorSetLayout> m_SetLayouts;
//     std::vector<VkPushConstantRange> m_Ranges;
//     bool m_Dirty = false;
// };
// }
