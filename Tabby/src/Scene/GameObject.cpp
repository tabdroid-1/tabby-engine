#include <Scene/GameObject.h>

namespace Tabby {

GameObject::GameObject(entt::entity handle, Scene* scene)
    : m_EntityHandle(handle)
    , m_Scene(scene)
{
}

}
