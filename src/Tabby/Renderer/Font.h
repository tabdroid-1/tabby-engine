#pragma once

#include <Tabby/Asset/AssetBase.h>
#include <Tabby/Renderer/Texture.h>

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
    // idk dude. raw pointer crashes when deleted.
    Shared<MSDFData> m_Data;
    AssetHandle m_AtlasTexture;
};

}
