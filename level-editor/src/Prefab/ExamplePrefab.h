#pragma once

#include <Tabby.h>
#include <Tabby/World/Prefab.h>

namespace Tabby {

class ExamplePrefab : public Prefab {
public:
    ExamplePrefab(AssetHandle handle)
        : Prefab(handle)
    {
        EntityData& rootEntity = GetRootEntity();

        TransformComponent tc;
        tc.Translation = { 3.442f, 4.231f, 123.2f };
        SpriteRendererComponent src;
        src.Color = { 1.0f, 0.0f, 0.5f, 0.45f };

        rootEntity.AddComponent<TransformComponent>(tc);
        rootEntity.AddComponent<SpriteRendererComponent>(src);
        EntityData& childEntity = rootEntity.AddChild();
        childEntity.AddComponent<TransformComponent>(tc);
        childEntity.AddComponent<SpriteRendererComponent>(src);
    }
};

}
