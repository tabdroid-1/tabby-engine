#pragma once

#if !defined TB_PLATFORM_WEB && !defined TB_PLATFORM_ANDROID
#include <filesystem>

#include "Tabby/Core/Base.h"
#include "Tabby/Renderer/Texture.h"

namespace Tabby {

struct MSDFData;

class Font {
public:
    Font(const std::filesystem::path& font);
    ~Font();

    const MSDFData* GetMSDFData() const { return m_Data; }
    Shared<Texture> GetAtlasTexture() const { return m_AtlasTexture; }

    static Shared<Font> GetDefault();

private:
    MSDFData* m_Data;
    Shared<Texture> m_AtlasTexture;
};

}
#endif
