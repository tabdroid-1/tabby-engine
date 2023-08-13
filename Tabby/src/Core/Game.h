#pragma once

#include <iostream>
#include <raylib.h>

#include "Game/Scenes/Scenes.h"
#include "Scene/SceneStateMachine.h"

class Game {
public:
    Game();

    void Init();
    void Update();
    void LateUpdate();
    void Draw();
    void Clean();

    bool IsRunning() const;

private:
    void SetupWindow();
    void ShutDownWindow();

private:
    RenderTexture2D frameBuffer;

    float deltaTime;
    // Window window;

    SceneStateMachine sceneManager;
};
