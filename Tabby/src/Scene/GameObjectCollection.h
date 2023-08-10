#pragma once

#include <cwchar>
#include <hash_map>
#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

// #include <Core/S_Drawable.h>
#include <Scene/GameObject.h>

class GameObjectCollection {
public:
    void Add(std::shared_ptr<GameObject> object);
    void Add(std::vector<std::shared_ptr<GameObject>> objects);
    GameObject& GetGameObject(const std::string& name);
    std::vector<std::shared_ptr<GameObject>> GetPersistentGameObjects();

    void Update(float deltaTime);
    void LateUpdate(float deltaTime);
    void Draw();

    void ProcessNewObjects();
    void ProcessRemovals();

private:
    std::vector<std::shared_ptr<GameObject>> gameObjects;
    std::vector<std::shared_ptr<GameObject>> newGameObjects;

    bool canGetObject = true;

    S_Drawable drawables;
};
