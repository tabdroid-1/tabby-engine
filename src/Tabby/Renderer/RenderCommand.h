#pragma once
#include <Tabby/Renderer/RendererAPI.h>

namespace Tabby {

class RenderCommand {
public:
    static void Init()
    {
        TB_PROFILE_SCOPE_NAME("Tabby::RendererCommand::Init");
        s_RendererAPI = RendererAPI::Create();
        s_RendererAPI->Init();
    }

    static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
    {
        TB_PROFILE_SCOPE_NAME("Tabby::RendererCommand::SetViewport");

        s_RendererAPI->SetViewport(x, y, width, height);
    }

    static void SetClearColor(const glm::vec4& color)
    {
        TB_PROFILE_SCOPE_NAME("Tabby::RendererCommand::SetClearColor");

        s_RendererAPI->SetClearColor(color);
    }

    static void Clear()
    {
        TB_PROFILE_SCOPE_NAME("Tabby::RendererCommand::Clear");

        s_RendererAPI->Clear();
    }

    static void DrawIndexed(const Shared<VertexArray>& vertexArray, uint32_t indexCount = 0)
    {
        TB_PROFILE_SCOPE_NAME("Tabby::RendererCommand::DrawIndexed");

        s_RendererAPI->DrawIndexed(vertexArray, indexCount);
    }

    static void DrawLines(const Shared<VertexArray>& vertexArray, uint32_t vertexCount)
    {
        TB_PROFILE_SCOPE_NAME("Tabby::RendererCommand::DrawLines");

        s_RendererAPI->DrawLines(vertexArray, vertexCount);
    }

    static void SetLineWidth(float width)
    {
        TB_PROFILE_SCOPE_NAME("Tabby::RendererCommand::SetLineWidth");

        s_RendererAPI->SetLineWidth(width);
    }

    static void EnableDepthTest(bool enable)
    {
        TB_PROFILE_SCOPE_NAME("Tabby::RendererCommand::EnableDepthTest");

        s_RendererAPI->EnableDepthTest(enable);
    }

    static void EnableAlphaBlending(bool enable)
    {
        TB_PROFILE_SCOPE_NAME("Tabby::RendererCommand::EnableAlphaBlending");

        s_RendererAPI->EnableAlphaBlending(enable);
    }

private:
    static Scope<RendererAPI> s_RendererAPI;
};

}
