#include <Tabby/Renderer/Image.h>

namespace Tabby {

Shared<Image> Image::Create(const ImageSpecification& spec, const AssetHandle& id)
{
    return CreateShared<Image>(spec, id);
}

Image::Image(const ImageSpecification& spec, const AssetHandle& id)
    : m_TextureHandle(BGFX_INVALID_HANDLE)
{
    Handle = id;
    Type = AssetType::TABBY_IMAGE;

    Update(spec);
};

Image::~Image()
{
}

void Image::Update(const ImageSpecification& spec)
{

    if (bgfx::isValid(m_TextureHandle)) {
        bgfx::destroy(m_TextureHandle);
        m_TextureHandle = BGFX_INVALID_HANDLE;
    }

    m_Specification = spec;

    uint64_t flags = 0
        | BGFX_SAMPLER_MIN_ANISOTROPIC
        | BGFX_SAMPLER_MAG_ANISOTROPIC
        | BGFX_SAMPLER_MIP_POINT
        | BGFX_SAMPLER_U_CLAMP
        | BGFX_SAMPLER_V_CLAMP;

    if (spec.usage == ImageUsage::RENDER_TARGET || spec.usage == ImageUsage::DEPTH_BUFFER) {
        flags |= BGFX_TEXTURE_RT;
    }

    m_TextureHandle = bgfx::createTexture2D(
        spec.extent.x,
        spec.extent.y,
        // spec.has_mips,
        false, // TODO: add mip support
        spec.array_layers,
        spec.format,
        flags);

    TB_CORE_ASSERT(bgfx::isValid(m_TextureHandle));

    if (!spec.pixels.empty()) {
        bgfx::updateTexture2D(
            m_TextureHandle,
            0, 0,
            0, 0, spec.extent.x, spec.extent.y,
            bgfx::copy(spec.pixels.data(), spec.pixels.size() * sizeof(uint8_t)));
    }
}

void Image::Destroy()
{
    if (bgfx::isValid(m_TextureHandle)) {
        bgfx::destroy(m_TextureHandle);
        m_TextureHandle = BGFX_INVALID_HANDLE;
    }
}

}
