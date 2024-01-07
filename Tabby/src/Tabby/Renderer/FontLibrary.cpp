// #include "Tabby/Renderer/FontLibrary.h"
// #include "freetype/freetype.h"
// #include "freetype/ftmodapi.h"
// #include <Tabby/Core/Base.h>
//
// namespace Tabby {
//
// FontLibrary* FontLibrary::s_Instance = nullptr;
// std::unordered_map<std::string, Ref<Font>> FontLibrary::m_Fonts;
// Ref<Font> FontLibrary::m_DefaultFont;
//
// FontLibrary::FontLibrary()
// {
//     TB_CORE_ASSERT(!s_Instance, "FontLibrary already exists!");
//     s_Instance = this;
//
//     FT_Init_FreeType(&m_FontLibrary);
//     TB_CORE_ASSERT(!m_FontLibrary, "FreeType initialization failed!");
//
//     m_DefaultFont = CreateRef<Font>(m_FontLibrary, "assets/fonts/opensans/OpenSans-Regular.ttf");
// }
//
// void FontLibrary::AddFontToLibrary(const std::string& name, std::string& path)
// {
//     Ref<Font> newFont = CreateRef<Font>(m_FontLibrary, path);
//
//     m_Fonts[name] = newFont;
// }
//
// Ref<Font> FontLibrary::FindFont(const std::string& name)
// {
//     if (!s_Instance) {
//         s_Instance = new FontLibrary();
//     }
//
//     return m_Fonts.find(name)->second;
// }
//
// Ref<Font> FontLibrary::GetDefaultFont()
// {
//     if (!s_Instance) {
//         s_Instance = new FontLibrary();
//     }
//
//     return m_DefaultFont;
// }
//
// FontLibrary::~FontLibrary()
// {
//     FT_Done_Library(m_FontLibrary);
// }
//
// }
