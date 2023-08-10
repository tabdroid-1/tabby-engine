#pragma once

#include <ComponentSystem/Components.h>
#include <Game/Scripts/Scripts.h>
#include <Scene/GameObjectCollection.h>
#include <Scene/Scene.h>
#include <Scene/SceneStateMachine.h>
#include <cwchar>
#include <memory>

class DevTestScene : public Scene {
public:
    DevTestScene(SceneStateMachine& sceneStateMachine);

    void OnCreate() override;
    void OnDestroy() override;
    void OnActivate() override;

    void Update(float deltaTime) override;
    void LateUpdate(float deltaTime) override;
    void Draw() override;

private:
    SceneStateMachine& sceneStateMachine;
    std::map<std::string, Camera2D> sceneCameras;
    Camera2D activeCamera;
};
