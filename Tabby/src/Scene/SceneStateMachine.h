#pragma once

#include <tbpch.h>

namespace Tabby {

class Scene;

class SceneStateMachine {

public:
    SceneStateMachine();

    void Update();
    void LateUpdate();
    void Draw();
    void DrawHud();
    void DrawImGui();

    unsigned int Add(std::shared_ptr<Tabby::Scene> scene);
    void SwitchTo(unsigned int id);
    void Remove(unsigned int id);

private:
    std::unordered_map<unsigned int, std::shared_ptr<Tabby::Scene>> scenes;
    std::shared_ptr<Tabby::Scene> curScene;
    unsigned int insertedSceneID;
};

}
