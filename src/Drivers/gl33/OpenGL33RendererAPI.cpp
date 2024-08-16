#include "Drivers/gl33/OpenGL33RendererAPI.h"

#include "tbpch.h"

#include <gl.h>
#include <Drivers/GPUProfiler.h>

namespace Tabby {

void OpenGL33RendererAPI::Init()
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL33RendererAPI::Init");
    TB_PROFILE_GPU("Tabby::OpenGL33RendererAPI::Init");

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
}

void OpenGL33RendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL33RendererAPI::SetViewport");
    TB_PROFILE_GPU("Tabby::OpenGL33RendererAPI::SetViewport");
    glViewport(x, y, width, height);
}

void OpenGL33RendererAPI::SetClearColor(const glm::vec4& color)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL33RendererAPI::SetClearColor");
    TB_PROFILE_GPU("Tabby::OpenGL33RendererAPI::SetClearColor");
    glClearColor(color.r, color.g, color.b, color.a);
}

void OpenGL33RendererAPI::Clear()
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL33RendererAPI::Clear");
    TB_PROFILE_GPU("Tabby::OpenGL33RendererAPI::Clear");
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGL33RendererAPI::DrawIndexed(const Shared<VertexArray>& vertexArray, uint32_t indexCount)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL33RendererAPI::DrawIndexed");
    TB_PROFILE_GPU("Tabby::OpenGL33RendererAPI::DrawIndexed");

    vertexArray->Bind();
    uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();
    glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
}

void OpenGL33RendererAPI::DrawLines(const Shared<VertexArray>& vertexArray, uint32_t vertexCount)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL33RendererAPI::DrawLines");
    TB_PROFILE_GPU("Tabby::OpenGL33RendererAPI::DrawLines");

    vertexArray->Bind();
    glDrawArrays(GL_LINES, 0, vertexCount);
}

void OpenGL33RendererAPI::SetLineWidth(float width)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL33RendererAPI::SetLineWidth");
    TB_PROFILE_GPU("Tabby::OpenGL33RendererAPI::SetLineWidth");

    glLineWidth(width);
}

void OpenGL33RendererAPI::EnableDepthTest(bool enable)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL33RendererAPI::EnableDepthTest");
    TB_PROFILE_GPU("Tabby::OpenGL33RendererAPI::EnableDepthTest");

    if (enable) {
        glEnable(GL_DEPTH_TEST);
    } else {
        glDisable(GL_DEPTH_TEST);
    }
}

void OpenGL33RendererAPI::EnableAlphaBlending(bool enable)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL33RendererAPI::EnableDepthTest");
    TB_PROFILE_GPU("Tabby::OpenGL33RendererAPI::EnableAlphaBlending");

    if (enable) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    } else {
        glDisable(GL_BLEND);
    }
}

}
