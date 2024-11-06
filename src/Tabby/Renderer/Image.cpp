#include <Tabby/Renderer/Image.h>

namespace Tabby {

Image::Image(const ImageSpecification& spec, const AssetHandle& id)
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
        | BGFX_SAMPLER_MIN_POINT
        | BGFX_SAMPLER_MAG_POINT
        | BGFX_SAMPLER_MIP_POINT
        | BGFX_SAMPLER_U_CLAMP
        | BGFX_SAMPLER_V_CLAMP;

    if (spec.usage == ImageUsage::RENDER_TARGET) {
        flags |= BGFX_TEXTURE_RT;
    } else if (spec.usage == ImageUsage::DEPTH_BUFFER) {
        flags |= BGFX_TEXTURE_RT_WRITE_ONLY;
    }

    m_TextureHandle = bgfx::createTexture2D(
        spec.extent.x,
        spec.extent.y,
        spec.mip_levels > 1,
        spec.array_layers,
        spec.format,
        flags);

    TB_CORE_ASSERT(bgfx::isValid(m_TextureHandle));

    /*if (!spec.pixels.empty()) {*/
    /*    bgfx::updateTexture2D(*/
    /*        m_TextureHandle,*/
    /*        0, 0,*/
    /*        0, 0, spec.extent.x, spec.extent.y,*/
    /*        bgfx::copy(spec.pixels.data(), spec.pixels.size()));*/
    /*}*/
}

void Image::Destroy()
{
    if (bgfx::isValid(m_TextureHandle)) {
        bgfx::destroy(m_TextureHandle);
        m_TextureHandle = BGFX_INVALID_HANDLE;
    }
}

}
