#include <tbpch.h>
#include <Drivers/null/NullAPIRendererAPI.h>

namespace Tabby {

void NullAPIRendererAPI::Init()
{
}

void NullAPIRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
}

void NullAPIRendererAPI::SetClearColor(const glm::vec4& color)
{
}

void NullAPIRendererAPI::Clear()
{
}

void NullAPIRendererAPI::DrawIndexed(const Shared<VertexArray>& vertexArray, uint32_t indexCount)
{
}

void NullAPIRendererAPI::DrawLines(const Shared<VertexArray>& vertexArray, uint32_t vertexCount)
{
}

void NullAPIRendererAPI::SetLineWidth(float width)
{
}

void NullAPIRendererAPI::EnableDepthTest(bool enable)
{
}

void NullAPIRendererAPI::EnableAlphaBlending(bool enable)
{
}

}
