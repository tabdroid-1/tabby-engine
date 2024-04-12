#include "SceneManager.h"

#include "Tabby/Scene/Scene.h"
#include <Tabby/Asset/AssetManager.h>

namespace Tabby {

SceneManager* SceneManager::s_Instance = nullptr;
std::pair<std::string, Shared<Scene>> SceneManager::curScene;
std::string SceneManager::nextSceneName = "";
entt::registry SceneManager::m_EntityRegistry;

SceneManager::SceneManager()
    : scenes(0)
{
    TB_CORE_ASSERT(!s_Instance, "Scene State Machine already exists!");
    s_Instance = this;
}

void SceneManager::Update(Timestep ts)
{
    if (!s_Instance) {
        s_Instance = new SceneManager();
    }

    if (s_Instance->curScene.second) {
        s_Instance->curScene.second->OnUpdate(ts);
    }
}

void SceneManager::DrawImGui()
{
    if (s_Instance->curScene.second) {
        s_Instance->curScene.second->DrawImGui();
    }
}

void SceneManager::Add(const std::string& sceneName, Shared<Tabby::Scene> scene)
{

    if (!s_Instance) {
        s_Instance = new SceneManager();
    }

    if (scene) {
        auto inserted = s_Instance->scenes.insert(std::make_pair(sceneName, scene));
        inserted.first->second->OnCreate();
        TB_CORE_TRACE("Scene {0} Added", sceneName);
    } else {
        TB_CORE_WARN("Failed to add Scene {0}. The key may already exist or the Scene pointer is invalid", sceneName);
    }
}

void SceneManager::Remove(const std::string& SceneName)
{

    auto it = s_Instance->scenes.find(SceneName);
    if (it != s_Instance->scenes.end()) {
        if (s_Instance->curScene.second == it->second) {
            s_Instance->curScene.second = nullptr;
        }

        s_Instance->scenes.erase(it);

        TB_CORE_TRACE("Scene {0} Removed", SceneName);
    } else {

        TB_CORE_ERROR("Scene {0} not found", SceneName);
    }
}

void SceneManager::SwitchTo(const std::string& SceneName)
{

    auto it = s_Instance->scenes.find(SceneName);
    if (it != s_Instance->scenes.end()) {

        if (s_Instance->curScene.second) {
            nextSceneName = it->first;
        } else {
            private_SwitchTo(SceneName);
        }
    }
}

void SceneManager::ProcessQueue(bool canSwitch)
{
    if (canSwitch && s_Instance->nextSceneName != "") {
        private_SwitchTo(s_Instance->nextSceneName);
        s_Instance->nextSceneName = "";
    }
}

void SceneManager::private_SwitchTo(std::string SceneName)
{

    auto it = s_Instance->scenes.find(SceneName);
    if (it != s_Instance->scenes.end()) {
        if (s_Instance->curScene.second) {
            s_Instance->curScene.second->OnStop();
        }

        s_Instance->curScene.first = it->first;
        s_Instance->curScene.second = it->second;

        TB_CORE_TRACE("Switched to scene {0}", SceneName);

        s_Instance->curScene.second->OnStart();
    } else {
        TB_CORE_ERROR("Scene {0} not found", SceneName);
    }
}

Shared<Tabby::Scene> SceneManager::GetCurrentScene()
{
    return curScene.second;
}

const std::string& SceneManager::GetCurrentSceneName()
{
    return curScene.first;
}

entt::registry& SceneManager::GetRegistry()
{
    return m_EntityRegistry;
}

}
