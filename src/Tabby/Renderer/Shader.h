#pragma once

#include "ShaderBufferLayout.h"
#include "Pipeline.h"
#include "Image.h"

namespace Tabby {

enum class ShaderStage : uint8_t {
    VERTEX,
    TASK,
    MESH,
    FRAGMENT,
    COMPUTE,
    UNKNOWN
};

struct ShaderSpecification {
    std::filesystem::path path;
    float line_width;
    std::string pipeline_debug_name;
    ShaderBufferLayout input_layout;
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

    static ShaderSpecification Default()
    {
        ShaderSpecification spec;
        spec.line_width = 1.0f;
        spec.culling_mode = PipelineCullingMode::BACK;
        spec.front_face = PipelineFrontFace::COUNTER_CLOCKWISE;
        spec.topology = PipelineTopology::TRIANGLES;
        spec.fill_mode = PipelineFillMode::FILL;
        spec.primitive_restart_enable = false;
        spec.color_blending_enable = true;
        spec.depth_test_enable = true;
        spec.multisampling_enable = false;
        spec.sample_count = 1;

        return spec;
    };
};

class Shader {
public:
    static Shared<Shader> Create(const ShaderSpecification& spec, std::map<ShaderStage, std::vector<uint32_t>> binaries);
    virtual ~Shader() {};
    virtual void Destroy() = 0;

    UUID GetID() const { return m_ID; }
    uint32_t GetVertexBufferSize() const { return m_VertexBufferSize; }

    // virtual void RestoreShaderModule(std::filesystem::path path) = 0;

    bool operator==(Shared<Shader> other) { return m_ID == other->m_ID; }

protected:
    uint32_t m_VertexBufferSize = 0;

    UUID m_ID;
};

namespace Utils {
    inline constexpr const char* ShaderStageToString(const ShaderStage& stage)
    {
        switch (stage) {
        case ShaderStage::VERTEX:
            return "vertex";
        case ShaderStage::FRAGMENT:
            return "fragment";
        case ShaderStage::COMPUTE:
            return "compute";
        case ShaderStage::TASK:
            return "task";
        case ShaderStage::MESH:
            return "mesh";
        case ShaderStage::UNKNOWN:
            return "unknown";
        default:
            TB_CORE_ASSERT(false);
            return "unknown";
        }
    }

}

}
