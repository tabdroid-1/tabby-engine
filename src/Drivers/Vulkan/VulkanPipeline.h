#pragma once

#include <Drivers/Vulkan/VulkanCommon.h>

#include <Tabby/Renderer/ShaderBufferLayout.h>
#include <Tabby/Renderer/Image.h>

namespace Tabby {

class Shader;

struct VulkanPipelineSpecification {
    std::string debug_name;
    ShaderBufferLayout input_layout;
    Shader* shader;
    float line_width;
    PipelineType type;
    PipelineCullingMode culling_mode;
    PipelineFrontFace front_face;
    PipelineTopology topology;
    PipelineFillMode fill_mode;
    std::vector<ImageFormat> output_attachments_formats;
    bool primitive_restart_enable;
    bool color_blending_enable;
    bool depth_test_enable;
    bool multisampling_enable;
    uint8_t sample_count;

    static VulkanPipelineSpecification Default()
    {
        VulkanPipelineSpecification spec = {};
        spec.debug_name = "";
        spec.input_layout = {};
        spec.shader = nullptr;
        spec.line_width = 1.0f;
        spec.type = PipelineType::GRAPHICS;
        spec.culling_mode = PipelineCullingMode::BACK;
        spec.front_face = PipelineFrontFace::COUNTER_CLOCKWISE;
        spec.topology = PipelineTopology::TRIANGLES;
        spec.fill_mode = PipelineFillMode::FILL;
        spec.output_attachments_formats = {};
        spec.primitive_restart_enable = false;
        spec.color_blending_enable = true;
        spec.depth_test_enable = true;
        spec.multisampling_enable = false;
        spec.sample_count = 1;

        return spec;
    }

    bool operator==(const VulkanPipelineSpecification& other) const
    {
        bool result = true;
        result &= shader == other.shader;
        result &= line_width == other.line_width;
        result &= type == other.type;
        result &= culling_mode == other.culling_mode;
        result &= front_face == other.front_face;
        result &= topology == other.topology;
        result &= fill_mode == other.fill_mode;
        result &= color_blending_enable == other.color_blending_enable;
        result &= depth_test_enable == other.depth_test_enable;
        result &= sample_count == other.sample_count;
        result &= input_layout.GetElements() == other.input_layout.GetElements();

        return result;
    }
};

class VulkanPipeline {
public:
    VulkanPipeline(const VulkanPipelineSpecification& spec);
    ~VulkanPipeline();

    void Destroy();

    VkPipeline Raw() const { return m_Pipeline; }
    VkPipelineLayout RawLayout() const { return m_PipelineLayout; }
    const VulkanPipelineSpecification& GetSpecification() const { return m_Specification; }

private:
    void CreateGraphics();
    void CreateCompute();

private:
    VulkanPipelineSpecification m_Specification;

    VkPipeline m_Pipeline;
    VkPipelineLayout m_PipelineLayout;
};

}
