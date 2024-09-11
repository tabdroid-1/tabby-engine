#pragma once

namespace Tabby {

// TODO: Make this an asset

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
    static Shared<Shader> Create(std::map<ShaderStage, std::vector<uint8_t>> binaries);
    virtual ~Shader() {};
    virtual void Destroy() = 0;

    UUID GetID() const { return m_ID; }

    // virtual void RestoreShaderModule(std::filesystem::path path) = 0;

    bool operator==(Shared<Shader> other) { return m_ID == other->m_ID; }

protected:
    UUID m_ID;
};

}
