#include "SceneStateMachine.h"

#include <Scene/Scene.h>

namespace Tabby {

SceneStateMachine::SceneStateMachine()
    : scenes(0)
    , curScene(0)
{
}

void SceneStateMachine::Update(float deltaTime)
{
    if (curScene) {
        curScene->Update(deltaTime);
    }
}

void SceneStateMachine::LateUpdate(float deltaTime)
{
    if (curScene) {
        curScene->LateUpdate(deltaTime);
    }
}

void SceneStateMachine::Draw()
{
    if (curScene) {
        curScene->Draw();
    }
}

void SceneStateMachine::DrawHud()
{
    if (curScene) {
        curScene->DrawHud();
    }
}

void SceneStateMachine::DrawImGui()
{
    if (curScene) {
        curScene->DrawImGui();
    }
}

unsigned int SceneStateMachine::Add(std::shared_ptr<Tabby::Scene> scene)
{
    auto inserted = scenes.insert(std::make_pair(insertedSceneID, scene));

    inserted.first->second->OnCreate();

    std::cout << "ID " << insertedSceneID << " Scene Added."
              << "\n";

    return insertedSceneID++;
}

void SceneStateMachine::Remove(unsigned int id)
{
    auto it = scenes.find(id);
    if (it != scenes.end()) {
        if (curScene == it->second) {
            curScene = nullptr;
        }

        it->second->OnDestroy();

        std::cout << "ID " << id << " Scene Removed."
                  << "\n";
        scenes.erase(it);
    } else {

        std::cout << "ID " << id << " Scene Not Found."
                  << "\n";
    }
}

void SceneStateMachine::SwitchTo(unsigned int id)
{

    // std::vector<std::shared_ptr<GameObject>> persistentGameObjects;

    if (curScene != 0) {
        // persistentGameObjects = curScene->gameObjects.GetPersistentGameObjects();
    }

    auto it = scenes.find(id);
    if (it != scenes.end()) {
        if (curScene) {
            curScene->OnDeactivate();
        }

        // if (!persistentGameObjects.empty()) {
        // it->second->gameObjects.Add(persistentGameObjects);
        // }
        curScene = it->second;

        std::cout << "Switched to Scene ID " << id << "\n";
        curScene->OnActivate();
        curScene->InitScene();
    }
}

}
