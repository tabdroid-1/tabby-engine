#include "Drivers/OpenGL33/OpenGL33Texture.h"
#include "tbpch.h"

#include <stb_image.h>

namespace Tabby {

OpenGL33Texture2D::OpenGL33Texture2D(uint32_t width, uint32_t height)
    : m_Width(width)
    , m_Height(height)
{
    TB_PROFILE_FUNCTION();

    m_InternalFormat = GL_RGBA8;
    m_DataFormat = GL_RGBA;

    // glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
    // glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);
    //
    // glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //
    // glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glGenTextures(1, &m_RendererID);

    glBindTexture(GL_TEXTURE_2D, m_RendererID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

OpenGL33Texture2D::OpenGL33Texture2D(const std::string& path)
    : m_Path(path)
{
    TB_PROFILE_FUNCTION();

    int width, height, channels;
    stbi_set_flip_vertically_on_load(1);
    stbi_uc* data = nullptr;
    {
        TB_PROFILE_SCOPE("stbi_load - OpenGL33Texture2D::OpenGL33Texture2D(const std::string&)");
        data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    }
    TB_CORE_ASSERT(data, "Failed to load image!");
    m_Width = width;
    m_Height = height;

    GLenum internalFormat = 0, dataFormat = 0;
    TB_CORE_INFO("Channels: {0}", channels);

    if (channels == 4) {
        internalFormat = GL_RGBA8;
        dataFormat = GL_RGBA;
    } else if (channels == 3) {
        internalFormat = GL_RGB8;
        dataFormat = GL_RGB;
    }

    m_InternalFormat = internalFormat;
    m_DataFormat = dataFormat;

    TB_CORE_ASSERT(internalFormat & dataFormat, "Format not supported!");

    // Create texture
    glGenTextures(1, &m_RendererID);
    glBindTexture(GL_TEXTURE_2D, m_RendererID);

    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Width, m_Height, 0, dataFormat, GL_UNSIGNED_BYTE, data);

    // Set texture filtering and wrapping options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glGenerateMipmap(GL_TEXTURE_2D);

    // glGenTextures(1, &m_RendererID);
    // glBindTexture(GL_TEXTURE_2D, m_RendererID);
    // // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    // // glGenerateMipmap(GL_TEXTURE_2D);
    //
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //
    // glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, m_Width, m_Height, 0, m_DataFormat, GL_UNSIGNED_BYTE, data);

    // glBindTexture(GL_TEXTURE_2D, m_RendererID);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    // glGenerateMipmap(GL_TEXTURE_2D);
    //
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //
    // glTexImage2D(m_RendererID, 1, internalFormat, m_Width, m_Height, 0, m_DataFormat, GL_UNSIGNED_BYTE, data);
    //
    //

    // glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
    // glTextureStorage2D(m_RendererID, 1, internalFormat, m_Width, m_Height);
    //
    // glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //
    // glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //
    // glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);
}

OpenGL33Texture2D::~OpenGL33Texture2D()
{
    TB_PROFILE_FUNCTION();

    glDeleteTextures(1, &m_RendererID);
}

void OpenGL33Texture2D::SetData(void* data, uint32_t size)
{
    TB_PROFILE_FUNCTION();

    uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;
    TB_CORE_ASSERT(size == m_Width * m_Height * bpp, "Data must be entire texture!");

    glBindTexture(GL_TEXTURE_2D, m_RendererID);
    glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, m_Width, m_Height, 0, m_DataFormat, GL_UNSIGNED_BYTE, data);
}

void OpenGL33Texture2D::Bind(uint32_t slot) const
{
    TB_PROFILE_FUNCTION();

    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_RendererID);
}
}
