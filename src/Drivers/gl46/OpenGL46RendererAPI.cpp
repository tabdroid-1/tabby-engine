#include <tbpch.h>
#include <Drivers/gl46/OpenGL46RendererAPI.h>
#include <Drivers/GPUProfiler.h>

#include <gl.h>

namespace Tabby {
void OpenGLMessageCallback(
    unsigned source,
    unsigned type,
    unsigned id,
    unsigned severity,
    int length,
    const char* message,
    const void* userParam)
{
    switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
        TB_CORE_CRITICAL(message);
        return;
    case GL_DEBUG_SEVERITY_MEDIUM:
        TB_CORE_ERROR(message);
        return;
    case GL_DEBUG_SEVERITY_LOW:
        TB_CORE_WARN(message);
        return;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        TB_CORE_TRACE(message);
        return;
    }

    TB_CORE_ASSERT_TAGGED(false, "Unknown severity level!");
}

void OpenGL46RendererAPI::Init()
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL46RendererAPI::Init");
    TB_PROFILE_GPU("Tabby::OpenGL46RendererAPI::Init");

#ifdef TB_DEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(OpenGLMessageCallback, nullptr);

    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
#endif

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
}

void OpenGL46RendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL46RendererAPI::SetViewport");
    TB_PROFILE_GPU("Tabby::OpenGL46RendererAPI::SetViewport");

    glViewport(x, y, width, height);
}

void OpenGL46RendererAPI::SetClearColor(const glm::vec4& color)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL46RendererAPI::SetClearColor");
    TB_PROFILE_GPU("Tabby::OpenGL46RendererAPI::SetClearColor");

    glClearColor(color.r, color.g, color.b, color.a);
}

void OpenGL46RendererAPI::Clear()
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL46RendererAPI::Clear");
    TB_PROFILE_GPU("Tabby::OpenGL46RendererAPI::Clear");

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGL46RendererAPI::DrawIndexed(const Shared<VertexArray>& vertexArray, uint32_t indexCount)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL46RendererAPI::DrawIndexed");
    TB_PROFILE_GPU("Tabby::OpenGL46RendererAPI::DrawIndexed");

    vertexArray->Bind();
    uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();
    glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
}

void OpenGL46RendererAPI::DrawLines(const Shared<VertexArray>& vertexArray, uint32_t vertexCount)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL46RendererAPI::DrawLines");
    TB_PROFILE_GPU("Tabby::OpenGL46RendererAPI::DrawLines");

    vertexArray->Bind();
    glDrawArrays(GL_LINES, 0, vertexCount);
}

void OpenGL46RendererAPI::SetLineWidth(float width)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL46RendererAPI::SetLineWidth");
    TB_PROFILE_GPU("Tabby::OpenGL46RendererAPI::SetLineWidth");

    glLineWidth(width);
}

void OpenGL46RendererAPI::EnableDepthTest(bool enable)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL46RendererAPI::EnableDepthTest");
    TB_PROFILE_GPU("Tabby::OpenGL46RendererAPI::EnableDepthTest");

    if (enable) {
        glEnable(GL_DEPTH_TEST);
    } else {
        glDisable(GL_DEPTH_TEST);
    }
}

void OpenGL46RendererAPI::EnableAlphaBlending(bool enable)
{
    TB_PROFILE_SCOPE_NAME("Tabby::OpenGL46RendererAPI::EnableDepthTest");
    TB_PROFILE_GPU("Tabby::OpenGL46RendererAPI::EnableAlphaBlending");

    if (enable) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    } else {
        glDisable(GL_BLEND);
    }
}

}
