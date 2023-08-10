#pragma once

#include <map>

#include <ComponentSystem/Components/C_Drawable.h>
#include <Scene/GameObject.h>
#include <unordered_map>
#include <vector>

class S_Drawable {
public:
    void Add(const std::vector<std::shared_ptr<GameObject>>& objects);
    void Add(std::shared_ptr<GameObject> object);
    void ProcessRemovals();

    void Draw();

private:
    void Sort();

    std::vector<std::shared_ptr<GameObject>> drawables;
};
