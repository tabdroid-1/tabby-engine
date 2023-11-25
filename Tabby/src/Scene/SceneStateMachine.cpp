#include "SceneStateMachine.h"

#include <Scene/Scene.h>

namespace Tabby {

SceneStateMachine* SceneStateMachine::s_Instance = nullptr;

SceneStateMachine::SceneStateMachine()
    : scenes(0)
    , curScene(0)
{
    TB_ASSERT(!s_Instance, "Scene State Machine already exists!");
    s_Instance = this;
}

void SceneStateMachine::Update()
{
    if (s_Instance->curScene) {
        s_Instance->curScene->Update();
    }
}

void SceneStateMachine::DrawHud()
{
    if (s_Instance->curScene) {
        s_Instance->curScene->DrawHud();
    }
}

void SceneStateMachine::DrawImGui()
{
    if (s_Instance->curScene) {
        s_Instance->curScene->DrawImGui();
    }
}

void SceneStateMachine::Add(std::string SceneName, std::shared_ptr<Tabby::Scene> Scene)
{
    auto inserted = s_Instance->scenes.insert(std::make_pair(SceneName, Scene));

    inserted.first->second->OnCreate();

    std::cout << "Scene \"" << SceneName << "\" Added."
              << "\n";
}

void SceneStateMachine::Remove(std::string SceneName)
{
    auto it = s_Instance->scenes.find(SceneName);
    if (it != s_Instance->scenes.end()) {
        if (s_Instance->curScene == it->second) {
            s_Instance->curScene = nullptr;
        }

        it->second->OnDestroy();

        std::cout << "ID \"" << SceneName << "\" Scene Removed."
                  << "\n";
        s_Instance->scenes.erase(it);
    } else {

        std::cout << "ID \"" << SceneName << "\" Scene Not Found."
                  << "\n";
    }
}

void SceneStateMachine::SwitchTo(std::string SceneName)
{

    // std::vector<std::shared_ptr<GameObject>> persistentGameObjects;

    if (s_Instance->curScene != 0) {
        // persistentGameObjects = curScene->gameObjects.GetPersistentGameObjects();
    }

    auto it = s_Instance->scenes.find(SceneName);
    if (it != s_Instance->scenes.end()) {
        if (s_Instance->curScene) {
            s_Instance->curScene->OnDeactivate();
        }

        // if (!persistentGameObjects.empty()) {
        // it->second->gameObjects.Add(persistentGameObjects);
        // }
        s_Instance->curScene = it->second;

        std::cout << "Switched to Scene \"" << SceneName << "\"\n";
        s_Instance->curScene->OnActivate();
        s_Instance->curScene->InitScene();
    }
}

}
