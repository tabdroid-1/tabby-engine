#pragma once

#include <Tabby/Renderer/ShaderBufferLayout.h>
#include <Tabby/Renderer/RenderPass.h>
#include <Tabby/Renderer/Image.h>

namespace Tabby {

enum class PipelineType : uint8_t {
    GRAPHICS,
    COMPUTE,
};

enum class PipelineCullingMode : uint8_t {
    BACK,
    FRONT,
    NONE
};

enum class PipelineFrontFace : uint8_t {
    CLOCKWISE,
    COUNTER_CLOCKWISE
};

enum class PipelineTopology : uint8_t {
    TRIANGLES,
    LINES,
    POINTS
};

enum class PipelineFillMode : uint8_t {
    FILL,
    EDGE_ONLY
};

struct PipelineSpecification {
    std::string debug_name;
    Shared<Shader> shader;
    Shared<RenderPass> render_pass;
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
    bool stencil_test_enable;
    bool multisampling_enable;
    uint8_t sample_count;

    static PipelineSpecification Default()
    {
        PipelineSpecification spec = {};
        spec.debug_name = "";
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
        spec.stencil_test_enable = true;
        spec.multisampling_enable = false;
        spec.sample_count = 1;

        return spec;
    }

    bool operator==(const PipelineSpecification& other) const
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

        return result;
    }
};

class Pipeline {
public:
    static Shared<Pipeline> Create(const PipelineSpecification& spec);
    virtual void Destroy() = 0;

    virtual const PipelineSpecification& GetSpecification() const = 0;
    UUID GetID() const { return m_ID; }

protected:
    UUID m_ID;
};

}
