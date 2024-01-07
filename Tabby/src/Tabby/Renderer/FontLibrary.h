// #pragma once
//
// #include <Tabby/Renderer/Font.h>
// #include <tbpch.h>
//
// #include <ft2build.h>
// #include FT_FREETYPE_H
//
// namespace Tabby {
//
// class FontLibrary {
// public:
//     FontLibrary();
//     ~FontLibrary();
//
//     void AddFontToLibrary(const std::string& name, std::string& path);
//
//     static Ref<Font> FindFont(const std::string& name);
//     static Ref<Font> GetDefaultFont();
//
// private:
//     static FontLibrary* s_Instance;
//     static std::unordered_map<std::string, Ref<Font>> m_Fonts;
//     static Ref<Font> m_DefaultFont;
//     FT_Library m_FontLibrary;
// };
//
// }
