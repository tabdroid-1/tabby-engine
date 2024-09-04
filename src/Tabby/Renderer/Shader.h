#pragma once

namespace Tabby {

// TODO: Make this an asset
class Shader {
public:
    static Shared<Shader> Create(std::vector<std::filesystem::path> files);
    virtual ~Shader() {};
    virtual void Destroy() = 0;

    UUID GetID() const { return m_ID; }

    // virtual void RestoreShaderModule(std::filesystem::path path) = 0;

    bool operator==(Shared<Shader> other) { return m_ID == other->m_ID; }

protected:
    UUID m_ID;
};

}
