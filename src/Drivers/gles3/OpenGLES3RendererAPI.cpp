#include "tbpch.h"

#include <Drivers/gles3/GLES3.h>
#include <Drivers/gles3/OpenGLES3RendererAPI.h>

#include <gles3.h>

namespace Tabby {

void OpenGLES3RendererAPI::Init()
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3RendererAPI::Init");

    GLES3::GL()->Enable(GL_BLEND);
    GLES3::GL()->BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLES3::GL()->Enable(GL_DEPTH_TEST);
}

void OpenGLES3RendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3RendererAPI::SetViewport");

    GLES3::GL()->Viewport(x, y, width, height);
}

void OpenGLES3RendererAPI::SetClearColor(const glm::vec4& color)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3RendererAPI::SetClearColor");

    GLES3::GL()->ClearColor(color.r, color.g, color.b, color.a);
}

void OpenGLES3RendererAPI::Clear()
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3RendererAPI::Clear");

    GLES3::GL()->Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLES3RendererAPI::DrawIndexed(const Shared<VertexArray>& vertexArray, uint32_t indexCount)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3RendererAPI::DrawIndexed");

    vertexArray->Bind();
    uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();
    GLES3::GL()->DrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
}

void OpenGLES3RendererAPI::DrawLines(const Shared<VertexArray>& vertexArray, uint32_t vertexCount)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3RendererAPI::DrawLines");

    vertexArray->Bind();
    GLES3::GL()->DrawArrays(GL_LINES, 0, vertexCount);
}

void OpenGLES3RendererAPI::SetLineWidth(float width)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3RendererAPI::SetLineWidth");

    GLES3::GL()->LineWidth(width);
}

void OpenGLES3RendererAPI::EnableDepthTest(bool enable)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3RendererAPI::EnableDepthTest");

    if (enable) {
        GLES3::GL()->Enable(GL_DEPTH_TEST);
    } else {
        GLES3::GL()->Disable(GL_DEPTH_TEST);
    }
}

void OpenGLES3RendererAPI::EnableAlphaBlending(bool enable)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGLES3RendererAPI::EnableDepthTest");

    if (enable) {
        GLES3::GL()->Enable(GL_BLEND);
        GLES3::GL()->BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    } else {
        GLES3::GL()->Disable(GL_BLEND);
    }
}

}
