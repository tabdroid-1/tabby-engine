#pragma once

#include <tbpch.h>
#include <Tabby/Asset/AssetBase.h>

namespace Tabby {

class AssetManager {
public:
    AssetManager();
    ~AssetManager();

    static void FullUnload();

    static AssetHandle LoadAssetSource(std::filesystem::path path, const AssetHandle& id = AssetHandle());
    static AssetHandle RegisterAsset(Shared<AssetBase> asset, const AssetHandle& id = AssetHandle());
    static bool HasAsset(AssetHandle id) { return m_AssetRegistry.find(id) != m_AssetRegistry.end(); }

    template <typename ResourceType> // where ResourceType is child of AssetType: Image, Mesh, Material etc.
    static Shared<ResourceType> GetAsset(AssetHandle id)
    {
        TB_PROFILE_SCOPE_NAME("Tabby::AssetManager::GetAsset");
        return std::static_pointer_cast<ResourceType>(m_AssetRegistry.at(id));
    }
    static const AssetHandle GetHandle(std::filesystem::path path) { return m_UUIDs.at(path.string()); }
    static const auto* GetAssetRegistry() { return &m_AssetRegistry; }

private:
    static AssetHandle ImportMeshSource(std::filesystem::path path, AssetHandle handle);
    static AssetHandle ImportImageSource(std::filesystem::path path, AssetHandle handle);
    static AssetHandle ImportFontSource(std::filesystem::path path, AssetHandle handle);
    static AssetHandle ImportAudioSource(std::filesystem::path path, AssetHandle handle);

private:
    inline static std::unordered_map<AssetHandle, Shared<AssetBase>> m_AssetRegistry;
    inline static std::unordered_map<std::string, UUID> m_UUIDs;
    inline static std::shared_mutex m_Mutex;
};

}
