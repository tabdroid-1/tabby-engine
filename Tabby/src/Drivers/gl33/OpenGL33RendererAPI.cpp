#include "Drivers/gl33/OpenGL33RendererAPI.h"
#include "Drivers/gl33/GL33.h"
#include "tbpch.h"

#include <glad/gl33.h>

namespace Tabby {

void OpenGL33RendererAPI::Init()
{
    TB_PROFILE_SCOPE();

    GL33::GL()->Enable(GL_BLEND);
    GL33::GL()->BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GL33::GL()->Enable(GL_DEPTH_TEST);
    GL33::GL()->Enable(GL_MULTISAMPLE);
}

void OpenGL33RendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
    GL33::GL()->Viewport(x, y, width, height);
}

void OpenGL33RendererAPI::SetClearColor(const glm::vec4& color)
{
    GL33::GL()->ClearColor(color.r, color.g, color.b, color.a);
}

void OpenGL33RendererAPI::Clear()
{
    GL33::GL()->Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGL33RendererAPI::DrawIndexed(const Shared<VertexArray>& vertexArray, uint32_t indexCount)
{
    TB_PROFILE_SCOPE();
    vertexArray->Bind();
    uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();
    GL33::GL()->DrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
}

void OpenGL33RendererAPI::DrawLines(const Shared<VertexArray>& vertexArray, uint32_t vertexCount)
{
    vertexArray->Bind();
    GL33::GL()->DrawArrays(GL_LINES, 0, vertexCount);
}

void OpenGL33RendererAPI::SetLineWidth(float width)
{
    GL33::GL()->LineWidth(width);
}

}
