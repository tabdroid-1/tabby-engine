#include <tbpch.h>
#include <Drivers/null/NullAPITexture.h>

namespace Tabby {

NullAPITexture::NullAPITexture(const TextureSpecification& specification, AssetHandle handle, Buffer data)
    : Texture(0)
    , m_Specification(specification)
    , m_Width(m_Specification.Width)
    , m_Height(m_Specification.Height)
{
}

NullAPITexture::~NullAPITexture()
{
}

void NullAPITexture::SetData(Buffer data)
{
}

void NullAPITexture::SetSubData(void* data, uint32_t width, uint32_t height)
{
}

void NullAPITexture::Bind(uint32_t slot) const
{
}

void NullAPITexture::Destroy()
{
}
}
