// #include "Sandbox2D.h"
// #include "Tabby/Asset/AssetManager.h"
// #include "Tabby/Renderer/Renderer2D.h"
// #include "glm/fwd.hpp"
// #include <imgui/imgui.h>
//
// #include <glm/gtc/matrix_transform.hpp>
// #include <glm/gtc/type_ptr.hpp>
//
// Sandbox2D::Sandbox2D()
//     : Layer("Sandbox2D")
//     , m_CameraController(1280.0f / 720.0f)
//     , m_SquareColor({ 0.2f, 0.3f, 0.8f, 1.0f })
// {
// }
//
// void Sandbox2D::OnAttach()
// {
//     TB_PROFILE_SCOPE();
//
//     Tabby::Renderer2D::SetLineWidth(10.0f);
//     m_CheckerboardTextureHandle = Tabby::AssetManager::Get()->LoadAssetSource("textures/Checkerboard.png", m_CheckerboardTextureHandle);
//     m_NoTextureHandle = Tabby::AssetManager::Get()->LoadAssetSource("textures/Checkerboard.png", m_CheckerboardTextureHandle);
//     m_CheckerboardTexture = Tabby::AssetManager::Get()->GetAsset<Tabby::Texture>(m_CheckerboardTextureHandle);
//     m_NoTexture = Tabby::AssetManager::Get()->GetAsset<Tabby::Texture>(m_NoTextureHandle);
// }
//
// void Sandbox2D::OnDetach()
// {
//     TB_PROFILE_SCOPE();
// }
//
// void Sandbox2D::OnUpdate(Tabby::Timestep ts)
// {
//     TB_PROFILE_SCOPE();
//
//     // Update
//     m_CameraController.OnUpdate(ts);
//
//     // Render
//     Tabby::Renderer2D::ResetStats();
//     {
//         TB_PROFILE_SCOPE_NAME("Renderer Prep");
//         Tabby::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
//         Tabby::RenderCommand::Clear();
//     }
//
//     {
//         static float rotation = 0.0f;
//         rotation += ts * 50.0f;
//
//         TB_PROFILE_SCOPE_NAME("Renderer Draw");
//         Tabby::Renderer2D::BeginScene(m_CameraController.GetCamera());
//         Tabby::Renderer2D::DrawRotatedQuad({ 1.0f, 0.0f }, { 0.8f, 0.8f }, -45.0f, { 0.8f, 0.2f, 0.3f, 1.0f });
//         Tabby::Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, { 0.8f, 0.2f, 0.3f, 1.0f });
//         Tabby::Renderer2D::DrawCircle({ -1.0f, 2.0f, 0.0f }, { 0.8f, 0.8f }, 0.0f, { 0.8f, 0.2f, 0.3f, 1.0f }, 3, 0.01);
//         Tabby::Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, m_SquareColor);
//         Tabby::Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f }, { 20.0f, 20.0f }, m_CheckerboardTexture, 10.0f);
//         Tabby::Renderer2D::DrawRotatedQuad({ -2.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }, rotation, m_NoTexture, 2.0f);
//         Tabby::Renderer2D::DrawRect({ -2.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }, { 0.8f, 0.2f, 0.3f, 1.0f }, 1);
//
//         glm::vec3 p2 = { -1.0f, 2.0f, 0.0f };
//         Tabby::Renderer2D::DrawLine({ 3.0f, -1.0f, 0.0f }, p2, { 0.8f, 0.2f, 0.3f, 1.0f }, -1);
//
//         Tabby::Renderer2D::EndScene();
//
//         Tabby::Renderer2D::BeginScene(m_CameraController.GetCamera());
//         for (float y = -5.0f; y < 5.0f; y += 0.5f) {
//             for (float x = -5.0f; x < 5.0f; x += 0.5f) {
//                 glm::vec4 color = { (x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.7f };
//                 Tabby::Renderer2D::DrawQuad({ x, y }, { 0.45f, 0.45f }, color);
//             }
//         }
//         Tabby::Renderer2D::EndScene();
//     }
// }
//
// void Sandbox2D::OnImGuiRender()
// {
//     TB_PROFILE_SCOPE();
//
//     ImGui::Begin("Settings");
//
//     auto stats = Tabby::Renderer2D::GetStats();
//     ImGui::Text("Renderer2D Stats:");
//     ImGui::Text("Draw Calls: %d", stats.DrawCalls);
//     ImGui::Text("Quads: %d", stats.QuadCount);
//     ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
//     ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
//
//     ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
//     ImGui::End();
// }
//
// void Sandbox2D::OnEvent(Tabby::Event& e)
// {
//     m_CameraController.OnEvent(e);
// }
