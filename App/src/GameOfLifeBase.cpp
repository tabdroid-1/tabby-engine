#include "GameOfLifeBase.h"
#include "Tabby/Renderer/Renderer2D.h"
#include "glm/fwd.hpp"
#include <GameOfLife.h>
#include <ctime>
#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

GameOfLife game(10, 10);

GameOfLifeBase::GameOfLifeBase()
    : Layer("GameOfLifeBase")
    , m_CameraController(1280.0f / 720.0f)
{
}

void GameOfLifeBase::OnAttach()
{
    TB_PROFILE_FUNCTION();

    Tabby::Renderer2D::SetLineWidth(10.0f);

    NoLifeTexture = Tabby::Texture2D::Create("assets/textures/NoLife.png");
    AliveTexture = Tabby::Texture2D::Create("assets/textures/alive.png");
}

void GameOfLifeBase::OnDetach()
{
    TB_PROFILE_FUNCTION();
}

float waitBeforeEvolution = 0.25f;
float evolutionTimer = 0;

void GameOfLifeBase::OnUpdate(Tabby::Timestep ts)
{
    TB_PROFILE_FUNCTION();

    // Update
    m_CameraController.OnUpdate(ts);

    // Render
    Tabby::Renderer2D::ResetStats();
    {
        TB_PROFILE_SCOPE("Renderer Prep");
        Tabby::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
        Tabby::RenderCommand::Clear();
    }

    {

        TB_PROFILE_SCOPE("Renderer Draw");
        Tabby::Renderer2D::BeginScene(m_CameraController.GetCamera());

        if (waitBeforeEvolution > evolutionTimer && (!game.IsStable() && game.GetGeneration() < 100)) {
            evolutionTimer += ts;
        } else {
            if (!game.IsStable())
                game.evolve();
            evolutionTimer = 0;
        }

        for (int x = 0; x < game.GetRows(); ++x) {
            for (int y = 0; y < game.GetColumn(); ++y) {
                if (game.isTileAlive(y, x)) {
                    Tabby::Renderer2D::DrawQuad({ x - game.GetRows() / 2, y - game.GetColumn() / 2, 0.0f }, { 1.0f, 1.0f }, AliveTexture);
                } else {
                    Tabby::Renderer2D::DrawQuad({ x - game.GetRows() / 2, y - game.GetColumn() / 2, 0.0f }, { 1.0f, 1.0f }, NoLifeTexture);
                }
            }
        }

        Tabby::Renderer2D::EndScene();
    }
}

void GameOfLifeBase::OnImGuiRender()
{
    TB_PROFILE_FUNCTION();

    ImGui::Begin("Info");

    ImGui::Text("I have decided to make asignement in my game engine.");
    ImGui::Text("Controls are WASD to move and Scroll to zoom in/out.");

    ImGui::End();

    ImGui::Begin("Stats");

    auto stats = Tabby::Renderer2D::GetStats();
    ImGui::Text("GameOfLife Stats:");
    ImGui::Text("Generation: %d", game.GetGeneration());
    ImGui::Text("Live Neighbors in row 2 column 19: %d", game.countLiveNeighbors(2, 19));
    ImGui::Text("Is stable?: %d", game.IsStable());
    if (game.IsStable()) {
        ImGui::Text("   Stable since generation: %d", game.GetStableGeneration());
    }

    ImGui::Text("");
    ImGui::Text("");
    ImGui::Text("");
    ImGui::Text("Renderer Stats:");
    ImGui::Text("Draw Calls: %d", stats.DrawCalls);
    ImGui::Text("Quads: %d", stats.QuadCount);
    ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
    ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

    ImGui::End();
}

void GameOfLifeBase::OnEvent(Tabby::Event& e)
{
    m_CameraController.OnEvent(e);
}
