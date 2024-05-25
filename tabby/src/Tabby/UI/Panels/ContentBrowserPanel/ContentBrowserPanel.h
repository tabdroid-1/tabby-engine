#pragma once

#include <Tabby/Renderer/Texture.h>

namespace Tabby {

class ContentBrowserPanel {
public:
    ContentBrowserPanel();

    void OnImGuiRender();

    void Open(bool open) { m_IsOpen = open; }
    bool IsOpen() { return m_IsOpen; }

private:
    bool m_IsOpen = true;

    std::filesystem::path m_BaseDirectory;
    std::filesystem::path m_CurrentDirectory;

    Shared<Texture> m_DirectoryIcon;
    Shared<Texture> m_FileIcon;
};

}
