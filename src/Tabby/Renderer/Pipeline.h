#pragma once

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

enum class ShaderDataType : uint32_t {
    INT,
    INT2,
    INT3,
    INT4,
    FLOAT,
    FLOAT2,
    FLOAT3,
    FLOAT4,
    IMAT3,
    IMAT4,
    MAT3,
    MAT4
};

constexpr uint32_t ShaderDataTypeSize(const ShaderDataType& type)
{
    switch (type) {
    case ShaderDataType::INT:
        return 4;
    case ShaderDataType::INT2:
        return 4 * 2;
    case ShaderDataType::INT3:
        return 4 * 3;
    case ShaderDataType::INT4:
        return 4 * 4;
    case ShaderDataType::FLOAT:
        return 4;
    case ShaderDataType::FLOAT2:
        return 4 * 2;
    case ShaderDataType::FLOAT3:
        return 4 * 3;
    case ShaderDataType::FLOAT4:
        return 4 * 4;
    case ShaderDataType::IMAT3:
        return 4 * 3 * 3;
    case ShaderDataType::IMAT4:
        return 4 * 4 * 4;
    case ShaderDataType::MAT3:
        return 4 * 3 * 3;
    case ShaderDataType::MAT4:
        return 4 * 4 * 4;
    default:
        TB_CORE_ASSERT(false);
        return 0;
    }
}

}
