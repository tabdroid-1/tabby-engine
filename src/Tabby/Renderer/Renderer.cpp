#include <Tabby/Renderer/ShaderLibrary.h>
#include <Tabby/Renderer/Renderer.h>
#include <Tabby/Renderer/Image.h>
#include <Tabby/Utils/Utils.h>

#include <Tabby/Core/Application.h>

#include <glm/gtc/packing.hpp>
#include <bx/math.h>

namespace Tabby {

static std::unordered_map<uint8_t, bgfx::FrameBufferHandle> s_FrameBuffers;
static std::unordered_map<uint8_t, ViewSpecification> s_ViewSpecifications;

void Renderer::Init()
{
    ShaderLibrary::Init();
}

void Renderer::Shutdown()
{
    ShaderLibrary::Shutdown();
}

void Renderer::SetViewTarget(const ViewSpecification& spec)
{
    // check if recreation is needed.
    if (s_ViewSpecifications.find(spec.view_id) != s_ViewSpecifications.end()) {
        if (s_ViewSpecifications.find(spec.view_id)->second == spec) {
            return;
        }
    }

    s_ViewSpecifications[spec.view_id] = spec;

    std::vector<bgfx::Attachment> framebuffer_attachments;

    if (s_FrameBuffers.find(spec.view_id) != s_FrameBuffers.end()) {
        if (bgfx::isValid(s_FrameBuffers.find(spec.view_id)->second)) {
            bgfx::destroy(s_FrameBuffers.find(spec.view_id)->second);
            s_FrameBuffers.erase(spec.view_id);
        }
    }

    for (auto render_target : spec.render_targets) {

        if (!render_target)
            continue;

        auto tex_spec = render_target->GetSpecification();

        if (tex_spec.extent != spec.render_area) {
            tex_spec.extent = spec.render_area;
            render_target->Update(tex_spec);
        }

        bgfx::Attachment framebuffer_attachment;
        framebuffer_attachment.init(render_target->Raw());
        framebuffer_attachments.push_back(framebuffer_attachment);
    }

    uint16_t flags = 0
        | BGFX_CLEAR_COLOR;

    if (spec.depth_buffer) {
        auto tex_spec = spec.depth_buffer->GetSpecification();

        if (tex_spec.extent != spec.render_area) {
            tex_spec.extent = spec.render_area;
            spec.depth_buffer->Update(tex_spec);
        }

        bgfx::Attachment framebuffer_attachment;
        framebuffer_attachment.init(spec.depth_buffer->Raw());
        framebuffer_attachments.push_back(framebuffer_attachment);

        flags |= BGFX_CLEAR_DEPTH;
    }

    s_FrameBuffers[spec.view_id] = bgfx::createFrameBuffer(framebuffer_attachments.size(), framebuffer_attachments.data(), false);

    bgfx::setViewFrameBuffer(spec.view_id, s_FrameBuffers[spec.view_id]);
    bgfx::setViewRect(spec.view_id, spec.offset.x, spec.offset.y, spec.render_area.x, spec.render_area.y);
    bgfx::setViewClear(spec.view_id, flags, Utils::ColorToHex(spec.clear_value), 1.0f, 0);
    bgfx::touch(spec.view_id);
}

void Renderer::SetViewMatrix(uint8_t view_id, const Matrix4& view, const Matrix4& projection)
{
    bgfx::setViewTransform(view_id, glm::value_ptr(view), glm::value_ptr(projection));
}

void Renderer::UnsetViewTarget(uint8_t view_id)
{
    if (s_FrameBuffers.find(view_id) != s_FrameBuffers.end()) {
        if (bgfx::isValid(s_FrameBuffers.find(view_id)->second)) {
            bgfx::destroy(s_FrameBuffers.find(view_id)->second);
            s_FrameBuffers.erase(view_id);
            s_ViewSpecifications.erase(view_id);
            bgfx::setViewFrameBuffer(view_id, BGFX_INVALID_HANDLE);
        }
    }
}

void Renderer::DrawMesh(uint8_t view_id, Shared<Mesh> mesh)
{

    bgfx::setTransform(glm::value_ptr(mesh->m_Transform));
    bgfx::setVertexBuffer(0, mesh->m_Specification.vertex_buffer_handle);
    bgfx::setIndexBuffer(mesh->m_Specification.index_buffer_handle);

    bgfx::setState(mesh->m_Specification.state);

    bgfx::submit(0, mesh->m_Specification.program_handle);
}

void Renderer::Frame()
{
    bgfx::frame();
}
}
