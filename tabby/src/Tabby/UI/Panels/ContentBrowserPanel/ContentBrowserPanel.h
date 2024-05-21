#pragma once

#include <Tabby/Renderer/Texture.h>

namespace Tabby {

class ContentBrowserPanel {
public:
    ContentBrowserPanel();

    void OnImGuiRender();

private:
    std::filesystem::path m_BaseDirectory;
    std::filesystem::path m_CurrentDirectory;

    Shared<Texture> m_DirectoryIcon;
    Shared<Texture> m_FileIcon;
};

}
