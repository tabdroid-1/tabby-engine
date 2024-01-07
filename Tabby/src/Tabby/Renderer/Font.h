// #pragma once
//
// #include <tbpch.h>
//
// #include "Tabby/Core/Base.h"
// #include "Tabby/Renderer/Texture.h"
// #include "freetype/freetype.h"
//
// namespace Tabby {
//
// struct Character {
//     Ref<Texture> Texture;
//     glm::ivec2 Size;
//     glm::ivec2 Bearing;
//     unsigned int Advance;
// };
//
// class Font {
// public:
//     Font(FT_Library& library, const std::string& fontPath);
//     ~Font();
//
//     Character GetCharacter(char character) { return m_Characters.find(character)->second; }
//
// private:
//     std::unordered_map<char, Character> m_Characters;
//     FT_Face m_Font;
// };
//
// }

#pragma once

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
    Ref<Texture2D> GetAtlasTexture() const { return m_AtlasTexture; }

    static Ref<Font> GetDefault();

private:
    MSDFData* m_Data;
    Ref<Texture2D> m_AtlasTexture;
};

}
