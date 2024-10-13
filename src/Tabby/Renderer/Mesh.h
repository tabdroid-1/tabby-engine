#pragma once

#include <Tabby/Foundation/Types.h>
#include <Tabby/Asset/AssetBase.h>

namespace Tabby {

class ShaderBuffer;
class Material;

enum class MeshFlags : BitMask {
    INDEX_TYPE_UINT8 = BIT(0),
    INDEX_TYPE_UINT16 = BIT(1),
    INDEX_TYPE_UINT32 = BIT(2),
};

struct MeshSpecification {
    std::string name = "Mesh";
    void* vertex_data;
    size_t vertex_data_size;
    void* index_data;
    size_t index_data_size;
    Shared<Material> material;
    BitMask flags;
};

class Mesh : public AssetBase {
public:
    Mesh(const MeshSpecification& spec, AssetHandle handle = AssetHandle());
    virtual ~Mesh() { }

    void Destroy();

    void SetMaterial(Shared<Material>);
    Shared<Material> GetMaterial();

    const std::string& GetName();
    const MeshSpecification& GetSpecification();
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
