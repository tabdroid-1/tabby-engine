#include "SceneStateMachine.h"

#include "Tabby/Scene/Scene.h"

namespace Tabby {

SceneStateMachine* SceneStateMachine::s_Instance = nullptr;
Ref<Tabby::Scene> SceneStateMachine::curScene;

SceneStateMachine::SceneStateMachine()
    : scenes(0)
{
    TB_CORE_ASSERT(!s_Instance, "Scene State Machine already exists!");
    s_Instance = this;
}

void SceneStateMachine::Update(Timestep ts)
{
    if (!s_Instance) {
        s_Instance = new SceneStateMachine();
    }

    if (s_Instance->curScene) {
        s_Instance->curScene->OnUpdate(ts);
    }
}

void SceneStateMachine::DrawImGui()
{
    if (s_Instance->curScene) {
        s_Instance->curScene->DrawImGui();
    }
}

void SceneStateMachine::Add(std::string SceneName, Ref<Tabby::Scene> Scene)
{

    if (!s_Instance) {
        s_Instance = new SceneStateMachine();
    }

    auto inserted = s_Instance->scenes.insert(std::make_pair(SceneName, Scene));

    if (inserted.second) {
        inserted.first->second->OnCreate();
        TB_INFO("Scene {0} Added", SceneName);
    } else {
        TB_WARN("Failed to add Scene {0}. The key may already exist or the Scene pointer is invalid", SceneName);
    }

    // auto inserted = s_Instance->scenes.insert(std::make_pair(SceneName, Scene));
    //
    // inserted.first->second->OnCreate();
    //
    // TB_INFO("Scene {0} Added.", SceneName);
}

void SceneStateMachine::Remove(std::string SceneName)
{

    auto it = s_Instance->scenes.find(SceneName);
    if (it != s_Instance->scenes.end()) {
        if (s_Instance->curScene == it->second) {
            s_Instance->curScene = nullptr;
        }

        s_Instance->scenes.erase(it);

        TB_INFO("Scene \"{0}\" Removed", SceneName);
    } else {

        TB_ERROR("Scene \"{0}\" not found", SceneName);
    }
}

void SceneStateMachine::SwitchTo(std::string SceneName)
{

    auto it = s_Instance->scenes.find(SceneName);
    if (it != s_Instance->scenes.end()) {
        if (s_Instance->curScene) {
            s_Instance->curScene->OnDeactivate();
        }

        s_Instance->curScene = it->second;

        TB_INFO("Switched to scene \"{0}\"", SceneName);
        s_Instance->curScene->OnActivate();
        s_Instance->curScene->OnStart();
    }
}

Tabby::Scene* SceneStateMachine::GetCurrentScene()
{
    return curScene.get();
}

}
