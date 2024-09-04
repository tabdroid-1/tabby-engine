#pragma once

#include <Drivers/Vulkan/VulkanCommon.h>
#include <Tabby/Renderer/Shader.h>

namespace Tabby {

class VulkanShader : public Shader {
public:
    VulkanShader(std::vector<std::filesystem::path> files);
    ~VulkanShader();

    void Destroy() override;

private:
    std::vector<VkPipelineShaderStageCreateInfo> m_StageCreateInfos;
    VkPipelineLayout m_PipelineLayout;
};

}
