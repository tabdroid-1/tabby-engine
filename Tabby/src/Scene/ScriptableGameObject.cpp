#include <Scene/Components.h>
#include <Scene/GameObject.h>
#include <Scene/SceneStateMachine.h>
#include <Scene/ScriptableGameObject.h>

namespace Tabby {

GameObject ScriptableEntity::FindGameObject(const std::string& GameObjectName)
{

    entt::entity entityHandle;
    auto scene = SceneStateMachine::GetCurrentScene();
    for (auto entity : scene->m_Registry.view<TagComponent>()) {

        auto& tag = scene->m_Registry.get<TagComponent>(entity);
        if (tag.Name == GameObjectName) {
            entityHandle = entity;
        }
    }

    return { entityHandle, SceneStateMachine::GetCurrentScene() };
}
}
