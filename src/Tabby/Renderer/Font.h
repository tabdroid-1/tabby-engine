#pragma once

#include <Tabby/Asset/AssetBase.h>

namespace Tabby {

// TODO: create tabby font format
struct FontSpecification {
    std::string name;
    std::string image_path;
    std::string json_path;
};

class Font : public AssetBase {
public:
    struct Atlas {
        AssetHandle texture;
        int distance_range;
        int distance_range_middle;
        float size;

        // Size of Atlas texture
        UIntVector2 extent;

        // Is y origin top or bottom
        bool y_origin_top = false;

        // no idea what this is.
        float em_size;
        float line_height;
        float ascender;
        float descender;
        float underline_y;
        float underline_thickness;
    };

    struct Character {
        uint32_t id = UINT32_MAX;
        uint32_t unicode_id = UINT32_MAX;
        double advance;

        float plane_bound_left;
        float plane_bound_bottom;
        float plane_bound_right;
        float plane_bound_top;

        float atlas_bound_left;
        float atlas_bound_bottom;
        float atlas_bound_right;
        float atlas_bound_top;

        operator bool() const
        {
            // unicode_id is the important part
            return unicode_id != UINT32_MAX;
            // return (id != UINT32_MAX) || (unicode_id != UINT32_MAX);
        }
    };

    static Shared<Font> Create(const FontSpecification& spec, const AssetHandle& id = AssetHandle());

    Font(const FontSpecification& spec, const AssetHandle& id = AssetHandle());
    ~Font();

    Atlas GetAtlasData() const { return m_Atlas; }
    Character GetCharacterByCodepoint(uint32_t character) const
    {
        auto c = m_CharactersByCodepoint.find(character);

        if (c != m_CharactersByCodepoint.end()) {
            return c->second;
        } else {
            return {};
        }
    }

    // TODO: kerning
    float GetCharacterAdvanceByCodepoint(uint32_t character1, uint32_t character2) const
    {
        auto c = m_CharactersByCodepoint.find(character1);

        if (c != m_CharactersByCodepoint.end()) {
            return c->second.advance;
        } else {
            return 0;
        }
    }

    void Destroy() override;

private:
    FontSpecification m_Specification;

    Atlas m_Atlas;
    std::unordered_map<uint32_t, Character> m_CharactersByIndex;
    std::unordered_map<uint32_t, Character> m_CharactersByCodepoint;
};

}
