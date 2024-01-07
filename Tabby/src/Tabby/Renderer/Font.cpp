// #include "Tabby/Renderer/Font.h"
// #include "freetype/freetype.h"
//
// #include "Tabby/Renderer/Texture.h"
// #include <Tabby/Core/Assert.h>
//
// namespace Tabby {
//
// Font::Font(FT_Library& library, const std::string& fontPath)
// {
//     FT_New_Face(library, fontPath.c_str(), 0, &m_Font);
//     TB_CORE_ASSERT(!m_Font, "Font initialization failed! (Perhaps you made typo?)");
//
//     // set size to load glyphs as
//     FT_Set_Pixel_Sizes(m_Font, 0, 48);
//
//     // load first 128 characters of ASCII set
//     for (unsigned char c = 0; c < 128; c++) {
//         // Load character glyph
//         bool successfull = FT_Load_Char(m_Font, c, FT_LOAD_RENDER);
//         TB_CORE_ASSERT(!successfull, "Failed to load character!")
//
//         // generate texture
//         TextureSpecification spec;
//         spec.Width = m_Font->glyph->bitmap.width;
//         spec.Height = m_Font->glyph->bitmap.rows;
//         spec.Format = ImageFormat::RGBA8;
//         spec.UnpackAlignment = 1;
//         spec.GenerateMips = false;
//
//         Ref<Texture2D> texture = Texture2D::Create(spec);
//
//         uint32_t size = spec.Width * spec.Height * 4;
//         texture->SetData((void*)m_Font->glyph->bitmap.buffer, size);
//
//         Character character
//             = {
//                   texture,
//                   glm::ivec2(m_Font->glyph->bitmap.width, m_Font->glyph->bitmap.rows),
//                   glm::ivec2(m_Font->glyph->bitmap_left, m_Font->glyph->bitmap_top),
//                   static_cast<unsigned int>(m_Font->glyph->advance.x)
//               };
//         m_Characters.insert(std::pair<char, Character>(c, character));
//     }
//
//     FT_Done_Face(m_Font);
// }
//
// Font::~Font()
// {
// }
//
// }

#include "Font.h"
#include "tbpch.h"

#undef INFINITE
#include "FontGeometry.h"
#include "GlyphGeometry.h"
#include "msdf-atlas-gen.h"

#include "MSDFData.h"

namespace Tabby {

template <typename T, typename S, int N, msdf_atlas::GeneratorFunction<S, N> GenFunc>
static Ref<Texture2D> CreateAndCacheAtlas(const std::string& fontName, float fontSize, const std::vector<msdf_atlas::GlyphGeometry>& glyphs,
    const msdf_atlas::FontGeometry& fontGeometry, uint32_t width, uint32_t height)
{
    msdf_atlas::GeneratorAttributes attributes;
    attributes.config.overlapSupport = true;
    attributes.scanlinePass = true;

    msdf_atlas::ImmediateAtlasGenerator<S, N, GenFunc, msdf_atlas::BitmapAtlasStorage<T, N>> generator(width, height);
    generator.setAttributes(attributes);
    generator.setThreadCount(8);
    generator.generate(glyphs.data(), (int)glyphs.size());

    msdfgen::BitmapConstRef<T, N> bitmap = (msdfgen::BitmapConstRef<T, N>)generator.atlasStorage();

    TextureSpecification spec;
    spec.Width = bitmap.width;
    spec.Height = bitmap.height;
    spec.Format = ImageFormat::RGB8;
    spec.GenerateMips = false;

    Ref<Texture2D> texture = Texture2D::Create(spec);
    texture->SetData((void*)bitmap.pixels, bitmap.width * bitmap.height * 3);
    return texture;
}

Font::Font(const std::filesystem::path& filepath)
    : m_Data(new MSDFData())
{
    msdfgen::FreetypeHandle* ft = msdfgen::initializeFreetype();
    TB_CORE_ASSERT(ft);

    std::string fileString = filepath.string();

    // TODO(Yan): msdfgen::loadFontData loads from memory buffer which we'll need
    msdfgen::FontHandle* font = msdfgen::loadFont(ft, fileString.c_str());
    if (!font) {
        TB_CORE_ERROR("Failed to load font: {}", fileString);
        return;
    }

    struct CharsetRange {
        uint32_t Begin, End;
    };

    // From imgui_draw.cpp
    static const CharsetRange charsetRanges[] = {
        { 0x0020, 0x00FF }
    };

    msdf_atlas::Charset charset;
    for (CharsetRange range : charsetRanges) {
        for (uint32_t c = range.Begin; c <= range.End; c++)
            charset.add(c);
    }

    double fontScale = 1.0;
    m_Data->FontGeometry = msdf_atlas::FontGeometry(&m_Data->Glyphs);
    int glyphsLoaded = m_Data->FontGeometry.loadCharset(font, fontScale, charset);
    TB_CORE_INFO("Loaded {} glyphs from font (out of {})", glyphsLoaded, charset.size());

    double emSize = 40.0;

    msdf_atlas::TightAtlasPacker atlasPacker;
    // atlasPacker.setDimensionsConstraint()
    atlasPacker.setPixelRange(2.0);
    atlasPacker.setMiterLimit(1.0);
    atlasPacker.setPadding(0);
    atlasPacker.setScale(emSize);
    int remaining = atlasPacker.pack(m_Data->Glyphs.data(), (int)m_Data->Glyphs.size());
    TB_CORE_ASSERT(remaining == 0);

    int width, height;
    atlasPacker.getDimensions(width, height);
    emSize = atlasPacker.getScale();

#define DEFAULT_ANGLE_THRESHOLD 3.0
#define LCG_MULTIPLIER 6364136223846793005ull
#define LCG_INCREMENT 1442695040888963407ull
#define THREAD_COUNT 8
    // if MSDF || MTSDF

    uint64_t coloringSeed = 0;
    bool expensiveColoring = false;
    if (expensiveColoring) {
        msdf_atlas::Workload([&glyphs = m_Data->Glyphs, &coloringSeed](int i, int threadNo) -> bool {
            unsigned long long glyphSeed = (LCG_MULTIPLIER * (coloringSeed ^ i) + LCG_INCREMENT) * !!coloringSeed;
            glyphs[i].edgeColoring(msdfgen::edgeColoringInkTrap, DEFAULT_ANGLE_THRESHOLD, glyphSeed);
            return true;
        },
            m_Data->Glyphs.size())
            .finish(THREAD_COUNT);
    } else {
        unsigned long long glyphSeed = coloringSeed;
        for (msdf_atlas::GlyphGeometry& glyph : m_Data->Glyphs) {
            glyphSeed *= LCG_MULTIPLIER;
            glyph.edgeColoring(msdfgen::edgeColoringInkTrap, DEFAULT_ANGLE_THRESHOLD, glyphSeed);
        }
    }

    m_AtlasTexture = CreateAndCacheAtlas<uint8_t, float, 3, msdf_atlas::msdfGenerator>("Test", (float)emSize, m_Data->Glyphs, m_Data->FontGeometry, width, height);

#if 0
		msdfgen::Shape shape;
		if (msdfgen::loadGlyph(shape, font, 'C'))
		{
			shape.normalize();
			//                      max. angle
			msdfgen::edgeColoringSimple(shape, 3.0);
			//           image width, height
			msdfgen::Bitmap<float, 3> msdf(32, 32);
			//                     range, scale, translation
			msdfgen::generateMSDF(msdf, shape, 4.0, 1.0, msdfgen::Vector2(4.0, 4.0));
			msdfgen::savePng(msdf, "output.png");
		}
#endif

    msdfgen::destroyFont(font);
    msdfgen::deinitializeFreetype(ft);
}

Font::~Font()
{
    delete m_Data;
}

Ref<Font> Font::GetDefault()
{
    static Ref<Font> DefaultFont;
    if (!DefaultFont)
        DefaultFont = CreateRef<Font>("assets/fonts/opensans/OpenSans-Regular.ttf");

    return DefaultFont;
}

}
