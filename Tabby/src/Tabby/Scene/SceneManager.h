#pragma once

#include <Tabby/Core/Time/Timestep.h>
#include <tbpch.h>

#include "entt/entt.hpp"

namespace Tabby {

class Scene;

class SceneManager {

public:
    SceneManager();

    static void Update(Timestep ts);
    static void DrawHud();
    static void DrawImGui();

    static void Add(const std::string& SceneName, Shared<Tabby::Scene> Scene);
    static void SwitchTo(const std::string& SceneName);
    static void Remove(const std::string& SceneName);

    static Shared<Scene> GetCurrentScene();
    static const std::string& GetCurrentSceneName();
    static entt::registry& GetRegistry();

private:
    static void ProcessQueue(bool canSwitch);
    static void private_SwitchTo(std::string SceneName);

private:
    std::unordered_map<std::string, Shared<Tabby::Scene>> scenes;
    static std::pair<std::string, Shared<Scene>> curScene;
    static std::string nextSceneName;

    static entt::registry m_EntityRegistry;

private:
    static SceneManager* s_Instance;
    friend class Scene;
};

}
