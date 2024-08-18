#pragma once
#include <Tabby/Renderer/Texture.h>
#include <Tabby/Asset/AssetBase.h>

namespace Tabby {

struct MSDFData;

class Font : public AssetBase {
public:
    Font(const std::string name, AssetHandle handle, Buffer data = Buffer());
    ~Font();

    const MSDFData* GetMSDFData() const { return m_Data.get(); }
    AssetHandle GetAtlasTexture() const { return m_AtlasTexture; }

    void Destroy() override;

private:
    Shared<MSDFData> m_Data; // idk dude. raw pointer crashes when deleted.
    AssetHandle m_AtlasTexture;
};

}
