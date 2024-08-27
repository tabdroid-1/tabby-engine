#include <tbpch.h>
#include <Tabby/Renderer/RendererAPI.h>
#include <Tabby/Asset/AssetManager.h>
#include <Tabby/Renderer/MSDFData.h>
#include <Tabby/Renderer/Font.h>

namespace Tabby {

template <typename T, typename S, int N, msdf_atlas::GeneratorFunction<S, N> GenFunc>
static AssetHandle CreateAndCacheFontAtlas(const std::string& fontName, float fontSize, const std::vector<msdf_atlas::GlyphGeometry>& glyphs,
    const msdf_atlas::FontGeometry& fontGeometry, uint32_t width, uint32_t height)
{

    TB_PROFILE_SCOPE_NAME("Tabby::CreateAndCacheFontAtlas");

    msdf_atlas::GeneratorAttributes attributes;
    attributes.config.overlapSupport = true;
    attributes.scanlinePass = true;

    msdf_atlas::ImmediateAtlasGenerator<S, N, GenFunc, msdf_atlas::BitmapAtlasStorage<T, N>> generator(width, height);
    generator.setAttributes(attributes);
    generator.setThreadCount(8);
    generator.generate(glyphs.data(), (int)glyphs.size());

    msdfgen::BitmapConstRef<T, N> bitmap = (msdfgen::BitmapConstRef<T, N>)generator.atlasStorage();

    // i have to do this goofy ass thing cus on ES 3, and only ES 3, texture is pitch black, unless its RGBA. i have no idea why.
    if (RendererAPI::GetAPI() == RendererAPI::API::OpenGLES3) {
        TextureSpecification spec;
        spec.Width = bitmap.width;
        spec.Height = bitmap.height;
        spec.Format = ImageFormat::RGBA8;
        spec.GenerateMips = false;
        spec.UnpackAlignment = 0;

        std::vector<uint8_t> rgbaData(bitmap.width * bitmap.height * 4);
        for (int i = 0, j = 0; i < bitmap.width * bitmap.height * 3; i += 3, j += 4) {
            rgbaData[j] = bitmap.pixels[i]; // R
            rgbaData[j + 1] = bitmap.pixels[i + 1]; // G
            rgbaData[j + 2] = bitmap.pixels[i + 2]; // B
            rgbaData[j + 3] = 255; // A (full opacity)
        }
        AssetHandle handle;
        Shared<Texture> texture = Texture::Create(spec, handle);
        texture->SetData(Buffer((void*)rgbaData.data(), rgbaData.size()));
        return AssetManager::RegisterAsset(texture, handle);
    } else {
        TextureSpecification spec;
        spec.Width = bitmap.width;
        spec.Height = bitmap.height;
        spec.Format = ImageFormat::RGB8;
        spec.GenerateMips = false;
        spec.UnpackAlignment = 4;

        AssetHandle handle;
        Shared<Texture> texture = Texture::Create(spec, handle);
        texture->SetData(Buffer((void*)bitmap.pixels, bitmap.width * bitmap.height * 3));
        return AssetManager::RegisterAsset(texture, handle);
    }
}

Font::Font(const std::string name, AssetHandle handle, Buffer data)
    : m_Data(CreateShared<MSDFData>())
{
#if !TB_HEADLESS
    Handle = handle;
    Type = AssetType::TABBY_FONT;

    msdfgen::FreetypeHandle* ft = msdfgen::initializeFreetype();
    TB_CORE_ASSERT(ft);

    msdfgen::FontHandle* font = msdfgen::loadFontData(ft, data.Data, data.Size);
    if (!font) {
        TB_CORE_ERROR("Failed to load font: {}", name);
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
    m_Data->FontGeometry.loadCharset(font, fontScale, charset);
    // TB_CORE_INFO("Loaded {} glyphs from font (out of {})", glyphsLoaded, charset.size());

    double emSize = 40.0;

    msdf_atlas::TightAtlasPacker atlasPacker;
    // atlasPacker.setDimensionsConstraint()
    atlasPacker.setPixelRange(2.0);
    atlasPacker.setMiterLimit(1.0);
    // atlasPacker.setPadding(0);
    atlasPacker.setSpacing(0);
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

    m_AtlasTexture = CreateAndCacheFontAtlas<uint8_t, float, 3, msdf_atlas::msdfGenerator>("Test", (float)emSize, m_Data->Glyphs, m_Data->FontGeometry, width, height);

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

#endif // !TB_HEADLESS
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
