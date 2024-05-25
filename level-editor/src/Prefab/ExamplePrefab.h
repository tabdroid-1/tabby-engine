#pragma once

#include <Tabby.h>
#include <Tabby/World/Prefab.h>

namespace Tabby {

class ExamplePrefab : public Prefab {
public:
    ExamplePrefab()
    {
        EntityData& rootEntity = GetRootEntity();

        TransformComponent tc;
        tc.Translation = { 3.442f, 4.231f, 123.2f };
        SpriteRendererComponent src;

        // AddComponent<TransformComponent>(parentEntity);
        // AddComponent<SpriteRendererComponent>(parentEntity);
    }
};

}
