#include <Tabby/Renderer/Renderer.h>
#include <Tabby/Renderer/Image.h>

#include <glm/gtc/packing.hpp>
#include <bx/math.h>

namespace Tabby {

inline static bool s_RenderPassActive = false;
inline static bgfx::FrameBufferHandle s_FrameBufferHandle = { bgfx::kInvalidHandle };

void Renderer::BeginRender(std::vector<Shared<Image>> render_targets, Shared<Image> depth_buffer, UIntVector2 render_area, IntVector2 offset, Vector4 clear_value)
{

    std::vector<bgfx::Attachment> framebuffer_attachments;
    constexpr uint64_t tsFlags = 0
        | BGFX_SAMPLER_MIN_POINT
        | BGFX_SAMPLER_MAG_POINT
        | BGFX_SAMPLER_MIP_POINT
        | BGFX_SAMPLER_U_CLAMP
        | BGFX_SAMPLER_V_CLAMP;

    if (bgfx::isValid(s_FrameBufferHandle)) {
        bgfx::destroy(s_FrameBufferHandle);
    }

    for (auto render_target : render_targets) {

        if (!render_target)
            continue;

        auto tex_spec = render_target->GetSpecification();
        tex_spec.extent = render_area;

        if (tex_spec.extent != render_area) {
            render_target->Update(tex_spec);
        }

        bgfx::Attachment framebuffer_attachment;
        framebuffer_attachment.init(render_target->Raw());
        framebuffer_attachments.push_back(framebuffer_attachment);
    }

    if (depth_buffer) {
        auto tex_spec = depth_buffer->GetSpecification();
        tex_spec.extent = render_area;

        if (tex_spec.extent != render_area) {
            depth_buffer->Update(tex_spec);
        }

        bgfx::Attachment framebuffer_attachment;
        framebuffer_attachment.init(depth_buffer->Raw());
        framebuffer_attachments.push_back(framebuffer_attachment);
    }

    s_FrameBufferHandle = bgfx::createFrameBuffer(framebuffer_attachments.size(), framebuffer_attachments.data(), false);

    const bx::Vec3 at = { 0.0f, 0.0f, 0.0f };
    const bx::Vec3 eye = { 0.0f, 0.0f, -35.0f };

    // Ready framebuffer for the frame
    {

        const bgfx::Caps* caps = bgfx::getCaps();

        float proj[16];
        float view[16];
        bx::mtxLookAt(view, eye, at);
        bx::mtxProj(proj, 60.0f, float(render_area.x) / float(render_area.x), 0.1f, 100.0f, caps->homogeneousDepth);

        bgfx::setViewTransform(0, view, proj);
        bgfx::setViewFrameBuffer(0, s_FrameBufferHandle);
        bgfx::setViewRect(0, 0, 0, render_area.x, render_area.x);

        uint16_t flags = 0
            | BGFX_CLEAR_COLOR;

        if (depth_buffer) {
            flags |= BGFX_CLEAR_DEPTH;
        }

        uint32_t clear_color_packed = glm::packSnorm3x10_1x2(clear_value);
        bgfx::setViewClear(0, flags, clear_color_packed, 1.0f, 0);
        bgfx::touch(0);
    }
}

void Renderer::EndRender()
{
    bgfx::setViewFrameBuffer(0, BGFX_INVALID_HANDLE);
}

void Renderer::Frame()
{
    bgfx::frame();
}
}
