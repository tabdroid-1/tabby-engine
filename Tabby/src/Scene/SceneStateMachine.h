#pragma once

#include <Scene/Scene.h>
#include <memory>
#include <unordered_map>

class SceneStateMachine {

public:
    SceneStateMachine();

    void Update(float deltaTime);
    void LateUpdate(float deltaTime);
    void Draw();

    unsigned int Add(std::shared_ptr<Tabby::Scene> scene);
    void SwitchTo(unsigned int id);
    void Remove(unsigned int id);

private:
    std::unordered_map<unsigned int, std::shared_ptr<Tabby::Scene>> scenes;
    std::shared_ptr<Tabby::Scene> curScene;
    unsigned int insertedSceneID;
};
