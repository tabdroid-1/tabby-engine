#include <Tabby/Asset/AssetManager.h>
#include <Tabby/Renderer/Font.h>
#include <Tabby/Renderer/Image.h>

#include <SDL_rwops.h>
#include <json.hpp>

namespace Tabby {

Shared<Font> Font::Create(const FontSpecification& spec, const AssetHandle& id)
{
    return CreateShared<Font>(spec, id);
}

Font::Font(const FontSpecification& spec, const AssetHandle& id)
{
    Handle = id;
    Type = AssetType::TABBY_FONT;

    m_Atlas.texture = AssetManager::LoadAssetSource(spec.image_path);

    // Parse json
    {
        std::string file_data;
        SDL_RWops* rw = SDL_RWFromFile(spec.json_path.c_str(), "rb");

        if (rw != nullptr) {
            Sint64 size = SDL_RWsize(rw);

            if (size > 0) {
                file_data.resize(size);
                Sint64 bytesRead = SDL_RWread(rw, file_data.data(), 1, size);
                if (bytesRead != size) {
                    TB_CORE_ERROR("Error reading file {0}", spec.image_path);
                    file_data.clear();
                    return;
                }
            }

            SDL_RWclose(rw);
        } else {
            TB_CORE_ERROR("Could not open file {0}", spec.image_path);
            return;
        }

        {

            auto json = nlohmann::json::parse(std::move(file_data));

            m_Atlas.distance_range = json["atlas"]["distanceRange"];
            m_Atlas.distance_range_middle = json["atlas"]["distanceRangeMiddle"];
            m_Atlas.size = json["atlas"]["size"];
            m_Atlas.extent.x = json["atlas"]["width"];
            m_Atlas.extent.y = json["atlas"]["height"];
            m_Atlas.y_origin_top = json["atlas"]["yOrigin"] == "bottom" ? false : true;
            m_Atlas.em_size = json["metrics"]["emSize"];
            m_Atlas.line_height = json["metrics"]["lineHeight"];
            m_Atlas.ascender = json["metrics"]["ascender"];
            m_Atlas.descender = json["metrics"]["descender"];
            m_Atlas.underline_y = json["metrics"]["underlineY"];
            m_Atlas.underline_thickness = json["metrics"]["underlineThickness"];

            for (auto element : json["glyphs"]) {
                Character character;
                if (element.contains("index"))
                    character.id = element["index"];
                if (element.contains("unicode"))
                    character.unicode_id = element["unicode"];
                else
                    TB_CORE_ERROR("\'{}\' does not contain unicode id", spec.json_path);

                character.advance = element["advance"];

                if (element.contains("planeBounds")) {
                    character.plane_bound_left = element["planeBounds"]["left"];
                    character.plane_bound_bottom = element["planeBounds"]["bottom"];
                    character.plane_bound_right = element["planeBounds"]["right"];
                    character.plane_bound_top = element["planeBounds"]["top"];
                } else {
                    character.plane_bound_left = 0;
                    character.plane_bound_bottom = 0;
                    character.plane_bound_right = 0;
                    character.plane_bound_top = 0;
                }

                if (element.contains("atlasBounds")) {
                    character.atlas_bound_left = element["atlasBounds"]["left"];
                    character.atlas_bound_bottom = element["atlasBounds"]["bottom"];
                    character.atlas_bound_right = element["atlasBounds"]["right"];
                    character.atlas_bound_top = element["atlasBounds"]["top"];
                } else {
                    character.atlas_bound_left = 0;
                    character.atlas_bound_bottom = 0;
                    character.atlas_bound_right = 0;
                    character.atlas_bound_top = 0;
                }

                m_CharactersByCodepoint[character.unicode_id] = character;
                m_CharactersByIndex[character.id] = character;
            }
        }
    }
}

Font::~Font()
{
    TB_PROFILE_SCOPE_NAME("Tabby::Font::Destructor");

    Destroy();
}

void Font::Destroy()
{
    TB_PROFILE_SCOPE_NAME("Tabby::Font::Destroy");
}
}
