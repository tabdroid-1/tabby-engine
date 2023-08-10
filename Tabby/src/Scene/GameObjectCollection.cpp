#include "GameObjectCollection.h"
#include "Scene/GameObject.h"
#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

void GameObjectCollection::Add(std::shared_ptr<GameObject> gameObject)
{

    for (auto object : gameObjects) {
        if (object->GetName() == gameObject->GetName()) {
            std::cout << "ERROR: There is already a gameobject called " << gameObject->GetName() << "\n";
            return;
        }
    }

    newGameObjects.push_back(gameObject);
}

void GameObjectCollection::Add(std::vector<std::shared_ptr<GameObject>> gameObjectsVector)
{
    if (!gameObjectsVector.empty()) {
        for (const auto& gameObject : gameObjectsVector) {

            static GameObject nullGameObject = GameObject("null");
            // if (GetGameObject(gameObject->GetName()).GetName() == nullGameObject.GetName()) {
            //     std::cout << "ERROR: There is already a gameobject called " << gameObject->GetName() << "\n";
            //     continue;
            // }
            newGameObjects.push_back(gameObject);
        }
    } else {
        std::cout << "WARNING: gameObjects Vector is empty! No Objects Added. \n";
    }
}

GameObject& GameObjectCollection::GetGameObject(const std::string& name)
{

    if (!gameObjects.empty()) {
        for (int i = 0; i < gameObjects.size(); i++) {
            if (gameObjects[i]) {
                if (gameObjects[i]->GetName() == name) {
                    return *gameObjects[i];
                }
            } else {
                std::cout << "gameObjects[" << i << "] is null!\n";
            }
        }
    }

    if (!newGameObjects.empty()) {
        for (int i = 0; i < newGameObjects.size(); i++) {
            if (newGameObjects[i]) {
                if (newGameObjects[i]->GetName() == name) {
                    return *newGameObjects[i];
                }
            } else {
                std::cout << "gameObjects[" << i << "] is null!\n";
            }
        }
    }

    // If the GameObject is not found, return a reference to a new GameObject (handle error case)
    static GameObject nullGameObject = GameObject("null");
    return nullGameObject;
}

// std::shared_ptr<GameObject> GameObjectCollection::GetGameObject(std::string name)
// {
//     for (const auto& gameObject : gameObjects) {
//         if (gameObject->GetName() == name) {
//             return gameObject;
//         }
//     }
//
//     return nullptr;
// }

std::vector<std::shared_ptr<GameObject>> GameObjectCollection::GetPersistentGameObjects()
{
    std::vector<std::shared_ptr<GameObject>> persistentGameObjects;

    if (!gameObjects.empty()) {
        for (const auto& gameObject : gameObjects) {
            if (gameObject->IsPersistent()) {
                persistentGameObjects.push_back(gameObject);
            }
        }
    }

    return persistentGameObjects;
}

void GameObjectCollection::Update(float deltaTime)
{
    for (const auto& o : gameObjects) {
        o->Update(deltaTime);
    }
}

void GameObjectCollection::LateUpdate(float deltaTime)
{
    for (const auto& o : gameObjects) {
        o->LateUpdate(deltaTime);
    }
}

void GameObjectCollection::Draw()
{
    drawables.Draw();
}

void GameObjectCollection::ProcessNewObjects()
{
    if (newGameObjects.size() > 0) {

        canGetObject = false;

        for (const auto& o : newGameObjects) {
            o->Awake();
        }

        for (const auto& o : newGameObjects) {
            o->Start();
        }

        gameObjects.insert(gameObjects.end(), newGameObjects.begin(), newGameObjects.end());

        drawables.Add(newGameObjects);

        newGameObjects.clear();

        canGetObject = true;
    }
}

void GameObjectCollection::ProcessRemovals()
{
    auto objIterator = gameObjects.begin();
    while (objIterator != gameObjects.end()) {
        auto obj = *objIterator;

        if (obj->IsQueuedForRemoval()) {
            objIterator = gameObjects.erase(objIterator);
        } else {
            ++objIterator;
        }
    }

    drawables.ProcessRemovals();
}
