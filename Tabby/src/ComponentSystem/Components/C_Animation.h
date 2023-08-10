#pragma once

#include <iostream>
#include <map>
#include <memory>
#include <string>

#include "C_Sprite.h"
#include "ComponentSystem/Component.h"
#include "Core/Animation.h"

// enum class AnimationState { None,
//     Idle,
//     Walk };

class C_Animation : public Component {
public:
    C_Animation(GameObject* owner);

    void Awake() override;

    void Update(float deltaTime) override;

    void AddAnimation(std::string Name, std::shared_ptr<Animation> Animation);
    void PlayAnimation(std::string Name);
    const std::string& GetAnimationName() const;

private:
    std::shared_ptr<C_Sprite> sprite;
    std::map<std::string, std::shared_ptr<Animation>> animations;
    std::pair<std::string, std::shared_ptr<Animation>> currentAnimation;
};

// enum class AnimationState { None,
//     Idle,
//     Walk };
//
// class C_Animation : public Component {
// public:
//     C_Animation(GameObject* owner);
//
//     void Awake() override;
//
//     void Update(float deltaTime) override;
//
//     void AddAnimation(AnimationState state, std::shared_ptr<Animation> animation);
//     void SetAnimationState(AnimationState state);
//     const AnimationState& GetAnimationState() const;
//
// private:
//     std::shared_ptr<C_Sprite> sprite;
//     std::map<AnimationState, std::shared_ptr<Animation>> animations;
//     std::pair<AnimationState, std::shared_ptr<Animation>> currentAnimation;
// };
