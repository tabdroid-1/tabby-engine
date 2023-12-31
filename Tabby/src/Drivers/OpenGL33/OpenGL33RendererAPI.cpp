#include "Drivers/OpenGL33/OpenGL33RendererAPI.h"
#include "tbpch.h"

#include <glad/gl33.h>

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
    // switch (severity) {
    // case GL_DEBUG_SEVERITY_HIGH:
    //     TB_CORE_CRITICAL(message);
    //     return;
    // case GL_DEBUG_SEVERITY_MEDIUM:
    //     TB_CORE_ERROR(message);
    //     return;
    // case GL_DEBUG_SEVERITY_LOW:
    //     TB_CORE_WARN(message);
    //     return;
    // case GL_DEBUG_SEVERITY_NOTIFICATION:
    //     TB_CORE_TRACE(message);
    //     return;
    // }

    TB_CORE_ASSERT(false, "Unknown severity level!");
}

void OpenGL33RendererAPI::Init()
{
    TB_PROFILE_FUNCTION();

    // #ifdef TB_DEBUG
    //     glEnable(GL_DEBUG_OUTPUT);
    //     glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    //     glDebugMessageCallback(OpenGLMessageCallback, nullptr);
    //
    //     glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
    // #endif

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);
}

void OpenGL33RendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
    glViewport(x, y, width, height);
}

void OpenGL33RendererAPI::SetClearColor(const glm::vec4& color)
{
    glClearColor(color.r, color.g, color.b, color.a);
}

void OpenGL33RendererAPI::Clear()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGL33RendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
{
    uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();
    glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
}

}
