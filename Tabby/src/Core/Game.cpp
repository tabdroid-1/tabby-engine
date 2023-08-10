#include "Game.h"
#include "Game/Scenes/DevTestScene.h"
#include "Game/Scenes/TitleScreenScene.h"
#include "raylib.h"
#include <cstdio>
#include <tuple>

Game::Game()
{
    Game::SetupWindow();
    frameBuffer = LoadRenderTexture(GetMonitorWidth(GetCurrentMonitor()), GetMonitorHeight(GetCurrentMonitor()));

    ToggleFullscreen();

    std::shared_ptr<TitleScreenScene> splashScreen = std::make_shared<TitleScreenScene>(sceneManager);
    std::shared_ptr<DevTestScene> gameScene = std::make_shared<DevTestScene>(sceneManager);

    unsigned int splashScreenID = sceneManager.Add(splashScreen);
    unsigned int gameSceneID = sceneManager.Add(gameScene);

    splashScreen->SetSwitchToScene(gameSceneID);

    sceneManager.SwitchTo(splashScreenID);
}

void Game::Update()
{
    deltaTime = GetFrameTime();

    sceneManager.Update(deltaTime);

    if (IsKeyDown(KEY_Q)) {
        CloseWindow();
        return;
    }

    Game::Draw();
}

void Game::LateUpdate() { sceneManager.LateUpdate(deltaTime); }

void Game::Draw()
{

    BeginTextureMode(frameBuffer);
    // textures and stuff goes here
    ClearBackground(BLACK);
    sceneManager.Draw();
    EndTextureMode();

    BeginDrawing();
    DrawTexturePro(frameBuffer.texture, { 0, 0, (float)frameBuffer.texture.width, -(float)frameBuffer.texture.height },
        { 0, 0, (float)GetMonitorWidth(GetCurrentMonitor()), (float)GetMonitorHeight(GetCurrentMonitor()) }, { 0, 0 }, 0, WHITE);
    EndDrawing();
}

void Game::Clean()
{
    UnloadRenderTexture(frameBuffer);
    Game::ShutDownWindow();
}

bool Game::IsRunning() const { return !WindowShouldClose(); }

void Game::SetupWindow()
{

    int monitor = GetCurrentMonitor();

    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(GetMonitorWidth(monitor), GetMonitorHeight(monitor), "Tabby2D InDev");

    int maxHeight = GetMonitorHeight(monitor) - 40;
    if (GetScreenHeight() > maxHeight)
        SetWindowSize(GetScreenWidth(), maxHeight);

    SetExitKey(0);
    SetTargetFPS(60);
}

void Game::ShutDownWindow() { CloseWindow(); }
