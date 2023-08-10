#include "Scene/GameObject.h"
#include <Core/S_Drawable.h>

#include <algorithm>
#include <cstdio>
#include <string>
#include <unordered_map>
#include <utility>

void S_Drawable::Add(const std::vector<std::shared_ptr<GameObject>>& gameObjects)
{
    for (const auto& o : gameObjects) {
        Add(o);
    }

    Sort();
}

void S_Drawable::ProcessRemovals()
{
    auto objIterator = drawables.begin();
    while (objIterator != drawables.end()) {
        auto obj = *objIterator;

        if (obj->IsQueuedForRemoval()) {
            objIterator = drawables.erase(objIterator);
        } else {
            ++objIterator;
        }
    }
}

void S_Drawable::Add(std::shared_ptr<GameObject> object)
{
    std::shared_ptr<C_Drawable> draw = object->GetDrawable();

    if (draw) {
        drawables.emplace_back(object);
    }
}

void S_Drawable::Sort()
{
    std::sort(drawables.begin(), drawables.end(), [](std::shared_ptr<GameObject> a, std::shared_ptr<GameObject> b) -> bool {
        return a->GetDrawable()->GetSortOrder() < b->GetDrawable()->GetSortOrder();
    });
}

void S_Drawable::Draw()
{
    for (auto& d : drawables) {
        d->Draw();
    }
}
