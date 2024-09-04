// #pragma once
//
// #include <Drivers/Vulkan/VulkanCommon.h>
//
// namespace Tabby {
// class VulkanPipeline {
// public:
//     VulkanPipeline();
//     ~VulkanPipeline();
//
//     void Destroy();
//
//     VkPipeline Raw() const { return m_Pipeline; }
//     VkPipelineLayout RawLayout() const { return m_PipelineLayout; }
//     // const PipelineSpecification& GetSpecification() const override { return m_Specification; }
//
// private:
//     void CreateGraphics();
//     void CreateCompute();
//
// private:
//     // PipelineSpecification m_Specification;
//
//     VkPipeline m_Pipeline;
//     VkPipelineLayout m_PipelineLayout;
// };
//
// }
