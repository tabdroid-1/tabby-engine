#include <Scene/Components.h>
#include <Scene/GameObject.h>

namespace Tabby {

GameObject::GameObject(entt::entity handle, Scene* scene)
    : m_EntityHandle(handle)
    , m_Scene(scene)
{
}

void GameObject::AddChild(GameObject& child)
{
    child.GetComponent<TransformComponent>().Parent = this->m_EntityHandle;
    this->GetComponent<TransformComponent>().Children.emplace_back(child.m_EntityHandle);
}

UUID GameObject::GetUUID()
{
    return GetComponent<IDComponent>().ID;
}

const std::string& GameObject::GetName()
{
    return GetComponent<TagComponent>().Name;
}

}
