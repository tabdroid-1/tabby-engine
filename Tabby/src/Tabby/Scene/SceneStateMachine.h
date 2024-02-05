#pragma once

#include <Tabby/Core/Timestep.h>
#include <tbpch.h>

namespace Tabby {

class Scene;

class SceneStateMachine {

public:
    SceneStateMachine();

    static void Update(Timestep ts);
    static void DrawHud();
    static void DrawImGui();

    static void Add(std::string SceneName, Ref<Tabby::Scene> Scene);
    static void SwitchTo(std::string SceneName);
    static void Remove(std::string SceneName);

    static Ref<Scene> GetCurrentScene();

private:
    std::unordered_map<std::string, Ref<Tabby::Scene>> scenes;
    static Ref<Scene> curScene;

private:
    static SceneStateMachine* s_Instance;
};

}
