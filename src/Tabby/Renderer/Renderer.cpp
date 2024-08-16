#include "Tabby/Renderer/Renderer.h"
#include "Tabby/Renderer/Renderer2D.h"
#include "tbpch.h"

namespace Tabby {

Scope<Renderer::SceneData> Renderer::s_SceneData = CreateScope<Renderer::SceneData>();

void Renderer::Init()
{
    TB_PROFILE_SCOPE_NAME("Tabby::Renderer::Init");

    RenderCommand::Init();
    Renderer2D::Init();
}

void Renderer::Shutdown()
{
    TB_PROFILE_SCOPE_NAME("Tabby::Renderer::Shutdown");

    Renderer2D::Shutdown();
}

void Renderer::OnWindowResize(uint32_t width, uint32_t height)
{
    TB_PROFILE_SCOPE_NAME("Tabby::Renderer::OnWindowResize");

    RenderCommand::SetViewport(0, 0, width, height);
}

void Renderer::Submit(const Shared<Shader>& shader, const Shared<VertexArray>& vertexArray, const glm::mat4& transform)
{
    TB_PROFILE_SCOPE_NAME("Tabby::Renderer::Submit");

    shader->Bind();
    shader->SetMat4("u_ViewProjection", s_SceneData->ViewProjectionMatrix);
    shader->SetMat4("u_Transform", transform);

    vertexArray->Bind();
    RenderCommand::DrawIndexed(vertexArray);
}

}
