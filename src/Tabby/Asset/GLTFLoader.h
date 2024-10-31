#pragma once

#include <Tabby/Asset/AssetManager.h>

#include <fastgltf/core.hpp>

namespace Tabby {

class Mesh;
class Image;

struct GLTFParseSpecification {
    std::filesystem::path filePath;
    bool create_entity_from_mesh = true;
};

struct GLTFLoader {
public:
    struct GLTFMaterialUniforms {
        Vector4 base_color_factor;
        float alpha_cutoff = 0.f;
        uint32_t flags = 0;
        Vector2 padding;
    };

    struct GLTFPrimitiveData {
        Shared<Mesh> primitive;
        std::unordered_map<std::string, Shared<Image>> images;
        uint32_t material_index;
        uint32_t mesh_index;
    };

    struct GLTFMeshData {
        std::vector<GLTFPrimitiveData> primitives;
    };

    struct GLTFData {
        std::vector<std::pair<uint32_t, Shared<Mesh>>> meshes;
        std::vector<GLTFMaterialUniforms> materials;
        std::vector<GLTFMeshData> mesh_data;
        std::vector<Shared<Image>> images;
    };

    static GLTFData Parse(GLTFParseSpecification spec);

private:
    struct Vertex {
        Vector3 position;
        Vector2 uv;
    };

    enum class MaterialUniformFlags : BitMask {
        None = BIT(0),
        HasAlbedoMap = BIT(1),
        HasNormalMap = BIT(2),
        HasRoughnessMap = BIT(3),
        HasOcclusionMap = BIT(4),
    };

    struct GLTFParserData {
        fastgltf::Asset fastgltf_asset;
        std::vector<std::pair<uint32_t, Shared<Mesh>>> meshes;
        std::vector<GLTFMaterialUniforms> materials;
        std::vector<GLTFMeshData> mesh_data;
        std::vector<Shared<Image>> images;
    };

    static void LoadImages(GLTFParserData& data);
    static void LoadMaterials(GLTFParserData& data);
    static void LoadMeshes(GLTFParserData& data, bool create_entity);
};

}
