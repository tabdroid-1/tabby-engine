#include "GameObject.h"
#include "raylib.h"

GameObject::GameObject(std::string name)
    : queuedForRemoval(false)
    , name(name)
    , isPersistent(false)
    , isChild(false)
{
    transform = AddComponent<C_Transform>();
}

GameObject::GameObject(std::string name, bool isPersistent)
    : queuedForRemoval(false)
    , name(name)
    , isPersistent(isPersistent)
    , isChild(false)
{
    transform = AddComponent<C_Transform>();
}

void GameObject::Awake()
{
    // for (int i = components.size() - 1; i >= 0; i--) {
    //     components[i]->Awake();
    // }

    for (const auto& c : components) {
        c->Awake();
    }

    for (const auto& c : children) {
        c->Awake();
    }
}

void GameObject::Start()
{
    // for (int i = components.size() - 1; i >= 0; i--) {
    //     components[i]->Start();
    // }

    for (const auto& c : components) {
        c->Start();
    }

    for (const auto& c : children) {
        c->Start();
    }
}

void GameObject::Update(float dt)
{
    // for (int i = components.size() - 1; i >= 0; i--) {
    //     components[i]->Update(timeDelta);
    // }

    for (const auto& c : components) {
        c->Update(dt);
    }

    for (const auto& c : children) {
        c->Update(dt);
    }
}

void GameObject::LateUpdate(float dt)
{
    // for (int i = components.size() - 1; i >= 0; i--) {
    //     components[i]->LateUpdate(timeDelta);
    // }

    for (const auto& c : components) {
        c->LateUpdate(dt);
    }

    for (const auto& c : children) {
        c->LateUpdate(dt);
        c->transform->SetPosition(transform->GetPosition());
        c->transform->SetScale(transform->GetScale());
        c->transform->SetRotation(transform->GetRotation());
    }
}

void GameObject::Draw()
{
    //
    // for (const auto& c : children) {
    //     c->drawable->Draw();
    // }

    if (!children.empty()) {
        drawables.Draw();
    }

    drawable->Draw();
}

void GameObject::Delete()
{
    // for (int i = components.size() - 1; i >= 0; i--) {
    //     components[i]->Delete();
    // }

    for (const auto& c : components) {
        c->Delete();
    }

    for (const auto& c : children) {
        c->Delete();
    }
}

std::shared_ptr<C_Drawable> GameObject::GetDrawable()
{
    return drawable;
}
void GameObject::QueueForRemoval() { queuedForRemoval = true; }

bool GameObject::IsQueuedForRemoval() { return queuedForRemoval; }
