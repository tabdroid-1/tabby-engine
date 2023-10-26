#pragma once

#include <Game/UI/SceneHierarchyPanel.h>
#include <Scene/Scene.h>
#include <Scene/SceneStateMachine.h>
#include <memory>

class TitleScreenScene : public Tabby::Scene {
public:
    TitleScreenScene(SceneStateMachine& sceneStateMachine);

    void OnCreate() override;
    void OnActivate() override;
    void OnDestroy() override;

    void DrawImGui() override;

    void SetSwitchToScene(unsigned int id);

private:
    SceneHierarchyPanel m_Panel;

private:
    SceneStateMachine& sceneStateMachine;

    float showForSeconds; // We want to show this scene for a set amount of time
    float currentSeconds; // How long the scene has currently been visible.

    unsigned int switchToState; // The state we want to transition to when this
                                // scenes time expires.
};
