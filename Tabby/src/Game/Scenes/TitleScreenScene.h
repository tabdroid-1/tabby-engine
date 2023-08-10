#pragma once

#include <ComponentSystem/Components.h>
#include <Game/Scripts/Scripts.h>
#include <Scene/GameObjectCollection.h>
#include <Scene/Scene.h>
#include <Scene/SceneStateMachine.h>
#include <memory>

class TitleScreenScene : public Scene {
public:
    TitleScreenScene(SceneStateMachine& sceneStateMachine);

    void OnCreate() override;
    void OnDestroy() override;

    void OnActivate() override;

    void SetSwitchToScene(unsigned int id);

    void Update(float deltaTime) override;
    void LateUpdate(float deltaTime) override;
    void Draw() override;

private:
    SceneStateMachine& sceneStateMachine;

    float showForSeconds; // We want to show this scene for a set amount of time
    float currentSeconds; // How long the scene has currently been visible.

    unsigned int switchToState; // The state we want to transition to when this
                                // scenes time expires.
};
