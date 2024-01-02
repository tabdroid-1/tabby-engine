#include "Drivers/gl33/OpenGL33Texture.h"
#include "tbpch.h"

#include <stb_image.h>

namespace Tabby {

namespace Utils {

    static GLenum TabbyImageFormatToGLDataFormat(ImageFormat format)
    {
        switch (format) {
        case ImageFormat::RGB8:
            return GL_RGB;
        case ImageFormat::RGBA8:
            return GL_RGBA;
        }

        TB_CORE_ASSERT(false);
        return 0;
    }

    static GLenum TabbyImageFormatToGLInternalFormat(ImageFormat format)
    {
        switch (format) {
        case ImageFormat::RGB8:
            return GL_RGB8;
        case ImageFormat::RGBA8:
            return GL_RGBA8;
        }

        TB_CORE_ASSERT(false);
        return 0;
    }

}

OpenGL33Texture2D::OpenGL33Texture2D(const TextureSpecification& specification)
    : m_Specification(specification)
    , m_Width(m_Specification.Width)
    , m_Height(m_Specification.Height)
{
    // TB_PROFILE_FUNCTION();

    m_InternalFormat = Utils::TabbyImageFormatToGLInternalFormat(m_Specification.Format);
    m_DataFormat = Utils::TabbyImageFormatToGLDataFormat(m_Specification.Format);

    // Create and bind a texture
    glGenTextures(1, &m_RendererID);
    glBindTexture(GL_TEXTURE_2D, m_RendererID);

    // Allocate storage for the texture using glTexImage2D
    glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
    // glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);
    //
    // glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //
    // glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

OpenGL33Texture2D::OpenGL33Texture2D(const std::string& path)
    : m_Path(path)
{
    // TB_PROFILE_FUNCTION();

    int width, height, channels;
    stbi_set_flip_vertically_on_load(1);
    stbi_uc* data = nullptr;
    {
        // TB_PROFILE_SCOPE("stbi_load - OpenGL33Texture2D::OpenGL33Texture2D(const std::string&)");
        data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    }

    if (data) {
        m_IsLoaded = true;

        m_Width = width;
        m_Height = height;

        GLenum internalFormat = 0, dataFormat = 0;
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

        // Generate and bind a texture
        glGenTextures(1, &m_RendererID);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);

        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Width, m_Height, 0, dataFormat, GL_UNSIGNED_BYTE, data);

        // Set texture filtering and wrapping options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // glGenerateMipmap(GL_TEXTURE_2D);

        // glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
        // glTextureStorage2D(m_RendererID, 1, internalFormat, m_Width, m_Height);
        //
        // glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        // glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        //
        // glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
        // glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
        //
        // glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);

        stbi_image_free(data);
    }
}

OpenGL33Texture2D::~OpenGL33Texture2D()
{
    // TB_PROFILE_FUNCTION();

    glDeleteTextures(1, &m_RendererID);
}

void OpenGL33Texture2D::SetData(void* data, uint32_t size)
{
    // TB_PROFILE_FUNCTION();

    // uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;
    // TB_CORE_ASSERT(size == m_Width * m_Height * bpp, "Data must be entire texture!");
    // glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);

    uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;
    // Ensure that the provided size matches the expected size
    TB_CORE_ASSERT(size == m_Width * m_Height * bpp, "Data must be the entire texture!");

    // Update the texture data using glTexSubImage2D
    glBindTexture(GL_TEXTURE_2D, m_RendererID);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
}

void OpenGL33Texture2D::Bind(uint32_t slot) const
{
    // TB_PROFILE_FUNCTION();

    // Activate a texture unit
    glActiveTexture(GL_TEXTURE0 + slot);

    // Bind the texture to the active texture unit
    glBindTexture(GL_TEXTURE_2D, m_RendererID);

    // glBindTextureUnit(slot, m_RendererID);
}
}
