#pragma once

namespace Tabby {

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

enum class ShaderStage : uint8_t {
    VERTEX,
    TASK,
    MESH,
    FRAGMENT,
    COMPUTE,
    UNKNOWN
};

class Shader {
public:
    static Shared<Shader> Create(std::map<ShaderStage, std::vector<uint32_t>> binaries, const std::string& path);
    virtual ~Shader() {};
    virtual void Destroy() = 0;

    UUID GetID() const { return m_ID; }
    uint32_t GetVertexBufferSize() const { return m_VertexBufferSize; }

    // virtual void RestoreShaderModule(std::filesystem::path path) = 0;

    bool operator==(Shared<Shader> other) { return m_ID == other->m_ID; }

protected:
    // ShaderBufferLayout m_VertexBufferLayout;
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
