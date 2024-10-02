#pragma once

#include <Tabby/Asset/AssetManager.h>

#include <fastgltf/core.hpp>

namespace Tabby {

class Mesh;
class Image;

struct GLTFLoader {
public:
    struct EntityGLTFMeshData {
        Shared<Mesh> mesh;
        std::unordered_map<std::string, Shared<Image>> images;

        Vector4 base_color_factor;
        float alpha_cutoff = 0.f;
        uint32_t flags = 0;
        Vector2 padding;
    };

    //                        Node name    Meshes assigned to node
    static std::vector<EntityGLTFMeshData> Parse(const std::filesystem::path& filePath);

private:
    struct Vertex {
        Vector3 position;
        Vector2 uv;
    };

    enum MaterialUniformFlags : uint32_t {
        None = 0 << 0,
        HasAlbedoMap = 1 << 0,
        HasNormalMap = 2 << 0,
        HasRoughnessMap = 3 << 0,
        HasOcclusionMap = 4 << 0,
    };

    struct GLTFParserData {
        fastgltf::Asset fastgltf_asset;
        std::vector<Shared<Image>> images;
        std::vector<EntityGLTFMeshData> materials;
    };

    static void LoadImages(GLTFParserData& data);
    static void LoadMaterials(GLTFParserData& data);
    static void LoadMeshes(GLTFParserData& data);
};

}
