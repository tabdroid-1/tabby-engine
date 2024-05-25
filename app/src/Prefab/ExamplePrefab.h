#pragma once

#include <Tabby.h>
#include <Tabby/World/Prefab.h>

namespace Tabby {

class ExamplePrefab : public Prefab {
public:
    ExamplePrefab()
        : Prefab(UUID())
    {
        EntityData& rootEntity = GetRootEntity();

        // TransformComponent tc;
        // tc.Translation = { 3.442f, 4.231f, 123.2f };
        // SpriteRendererComponent src;
        // src.Color = { 1.0f, 0.0f, 0.5f, 0.45f };

        rootEntity.AddComponent<TransformComponent>(glm::vec3(3.442f, 4.231f, 123.2f));
        rootEntity.AddComponent<SpriteRendererComponent>(glm::vec4(1.0f, 0.0f, 0.5f, 0.45f));
        EntityData& childEntity = rootEntity.AddChild();
        childEntity.AddComponent<TransformComponent>(glm::vec3(3.442f, 4.231f, 123.2f));
        childEntity.AddComponent<SpriteRendererComponent>(glm::vec4(1.0f, 0.0f, 0.5f, 0.45f));
    }
};

}
