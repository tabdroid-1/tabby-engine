#pragma once

#include <Tabby/Asset/AssetBase.h>
#include <Tabby/Renderer/Texture.h>

#include <freetype/freetype.h>

namespace Tabby {

struct FontSpecification {
    std::filesystem::path path;
    int glyphSize = 48;
};

struct Character {
    Shared<Texture> Texture; // The glyph texture
    glm::ivec2 Size; // Size of glyph
    glm::ivec2 Bearing; // Offset from baseline to left/top of glyph
    unsigned int Advance; // Horizontal offset to advance to next glyph
};

class Font : public AssetBase {

public:
    Font(const FontSpecification& specification, AssetHandle handle, Buffer data = Buffer());
    ~Font();

    Shared<Texture> GetAtlasTexture() const { return m_AtlasTexture; }

    void Destroy() override;

private:
    FT_Face* m_Face;
    FontSpecification m_Specification;
    Shared<Texture> m_AtlasTexture;
};

}
