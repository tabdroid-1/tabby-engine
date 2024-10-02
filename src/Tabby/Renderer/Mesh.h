#pragma once

#include <Tabby/Foundation/Types.h>
#include <Tabby/Asset/AssetBase.h>

namespace Tabby {

class ShaderBuffer;
class Material;

enum class MeshIndexType {
    UINT8 = 0,
    UINT16,
    UINT32,
};

struct MeshSpecification {
    std::string name = "Mesh";
    Buffer vertex_data;
    Buffer index_data;
    Shared<Material> material;
    MeshIndexType index_type = MeshIndexType::UINT16;
};

class Mesh : public AssetBase {
public:
    Mesh(const MeshSpecification& spec, AssetHandle handle = AssetHandle());
    virtual ~Mesh() { }

    void Destroy();

    void SetMaterial(Shared<Material>);
    Shared<Material> GetMaterial();

    const std::string& GetName();
    Shared<ShaderBuffer> GetVertexBuffer();
    Shared<ShaderBuffer> GetIndexBuffer();
    uint32_t TotalIndexCount();

private:
    MeshSpecification m_Specification;

    uint32_t m_TotalIndexCount = 0;
    Shared<ShaderBuffer> m_VertexBuffer;
    Shared<ShaderBuffer> m_IndexBuffer;

    friend class VulkanRenderer;
};
}
