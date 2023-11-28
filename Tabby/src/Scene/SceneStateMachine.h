#pragma once

#include <tbpch.h>

namespace Tabby {

class Scene;

class SceneStateMachine {

public:
    SceneStateMachine();

    static void Update();
    static void DrawHud();
    static void DrawImGui();

    static void Add(std::string SceneName, std::shared_ptr<Tabby::Scene> Scene);
    static void SwitchTo(std::string SceneName);
    static void Remove(std::string SceneName);

    static Tabby::Scene* GetCurrentScene();

private:
    std::unordered_map<std::string, std::shared_ptr<Tabby::Scene>> scenes;
    static std::shared_ptr<Tabby::Scene> curScene;

private:
    static SceneStateMachine* s_Instance;
};

}
