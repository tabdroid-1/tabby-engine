#pragma once

namespace Tabby {

enum class AssetType : uint8_t {
    FONT_SRC,
    MESH_SRC,
    MATERIAL,
    AUDIO_SRC,
    IMAGE_SRC,
    TABBY_FONT,
    TABBY_MESH,
    TABBY_MODEL,
    TABBY_IMAGE,
    TABBY_AUDIO,
    TABBY_MATERIAL,
    UNKNOWN
};

inline static const std::unordered_map<std::string, AssetType> g_AssetTypesExtensionMap = {
    { ".ttf", AssetType::FONT_SRC },

    { ".png", AssetType::IMAGE_SRC },
    { ".jpg", AssetType::IMAGE_SRC },
    { ".jpeg", AssetType::IMAGE_SRC },
    { ".tga", AssetType::IMAGE_SRC },

    { ".fbx", AssetType::MESH_SRC },
    { ".gltf", AssetType::MESH_SRC },
    { ".glb", AssetType::MESH_SRC },
    { ".obj", AssetType::MESH_SRC },

    { ".wav", AssetType::AUDIO_SRC },

};

inline AssetType FileExtensionToAssetType(std::string_view extension)
{
    if (g_AssetTypesExtensionMap.find(extension.data()) != g_AssetTypesExtensionMap.end())
        return g_AssetTypesExtensionMap.at(extension.data());

    return AssetType::UNKNOWN;
}

}
