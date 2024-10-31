#pragma once

#include <Drivers/Vulkan/VulkanCommon.h>

#include <Tabby/Renderer/Pipeline.h>

namespace Tabby {

class Shader;
class VulkanRenderPass;

class VulkanPipeline : public Pipeline {
public:
    VulkanPipeline(const PipelineSpecification& spec);
    ~VulkanPipeline();

    void Destroy() override;
    const PipelineSpecification& GetSpecification() const override { return m_Specification; }

    VkPipeline Raw() const { return m_Pipeline; }
    VkPipelineLayout RawLayout() const { return m_PipelineLayout; }

private:
    void CreateGraphics();
    void CreateCompute();

private:
    PipelineSpecification m_Specification;

    VkPipeline m_Pipeline;
    VkPipelineLayout m_PipelineLayout;
};

}
