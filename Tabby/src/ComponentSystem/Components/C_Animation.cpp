#include "C_Animation.h"
#include "Scene/GameObject.h"
#include <cstdio>
#include <iostream>

C_Animation::C_Animation(GameObject* owner)
    : Component(owner)
    , currentAnimation("null", nullptr)
{
}

void C_Animation::Awake() { sprite = owner->GetComponent<C_Sprite>(); }

void C_Animation::Update(float deltaTime)
{
    if (currentAnimation.first != "null") {
        bool newFrame = currentAnimation.second->UpdateFrame(deltaTime);

        if (newFrame) {
            const FrameData* data = currentAnimation.second->GetCurrentFrame();
            /* sprite->Load(data->texture); */

            sprite->SetTextureRect(data->x, data->y, data->width, data->height);
        }
    }
}

void C_Animation::AddAnimation(std::string Name, std::shared_ptr<Animation> Animation)
{
    auto inserted = animations.insert(std::make_pair(Name, Animation));

    if (currentAnimation.first == "null") {
        PlayAnimation(Name);
    }
}

void C_Animation::PlayAnimation(std::string Name)
{
    if (currentAnimation.first == Name) {
        return;
    }

    auto animation = animations.find(Name);
    if (animation != animations.end()) {
        currentAnimation.first = animation->first;
        currentAnimation.second = animation->second;

        owner->GetComponent<C_Sprite>()->Load(animation->second->GetCurrentFrame()->texturePath, true);
        currentAnimation.second->Reset();
    }
}

const std::string& C_Animation::GetAnimationName() const
{
    return currentAnimation.first;
}
