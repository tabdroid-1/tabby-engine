#pragma once

#include <memory>
#include <unordered_map>

#include "Scene.h"
#include "Util/EnumHash.h"

class SceneStateMachine {
public:
    SceneStateMachine();

    void Update(float deltaTime);
    void LateUpdate(float deltaTime);
    void Draw();

    unsigned int Add(std::shared_ptr<Scene> scene);
    void SwitchTo(unsigned int id);
    void Remove(unsigned int id);

private:
    std::unordered_map<unsigned int, std::shared_ptr<Scene>> scenes;
    std::shared_ptr<Scene> curScene;
    unsigned int insertedSceneID;
};
