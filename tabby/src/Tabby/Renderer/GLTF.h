#pragma once

#include <Tabby/Asset/AssetManager.h>

#include <fastgltf/core.hpp>

namespace Tabby {

class Mesh;
class Texture;
// class Material;

class GLTF {
public:
    GLTF(const std::filesystem::path& filePath);

    void Draw();

    std::vector<Shared<Mesh>> m_Meshes;

private:
    void LoadImages();
    void LoadMaterials();
    void LoadMeshes();
    void LoadCamera();

    enum MaterialUniformFlags : std::uint32_t {
        None = 0 << 0,
        HasAlbedoMap = 1 << 0,
        HasNormalMap = 2 << 0,
        HasRoughnessMap = 3 << 0,
        HasOcclusionMap = 4 << 0,
    };

    struct MaterialUniforms {
        Vector4 baseColorFactor;
        float alphaCutoff = 0.f;
        uint32_t flags = 0;

        Vector2 padding;
    };

private:
    fastgltf::Asset m_Asset;
    std::vector<Shared<Texture>> m_Images;
    std::vector<MaterialUniforms> m_Materials;
    // std::vector<Shared<Material>> m_Materials;
};

}
