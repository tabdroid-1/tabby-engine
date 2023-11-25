#pragma once

#include <Game/UI/SceneHierarchyPanel.h>
#include <Scene/Scene.h>
#include <Scene/SceneStateMachine.h>
#include <memory>

class TitleScreenScene : public Tabby::Scene {
public:
    TitleScreenScene();

    void OnCreate() override;
    void OnActivate() override;
    void OnDestroy() override;

    void DrawHud() override;
    void DrawImGui() override;

    void SetSwitchToScene(unsigned int id);

private:
    SceneHierarchyPanel m_Panel;
};
