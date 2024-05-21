#pragma once

#include <Tabby.h>

#include "glm/fwd.hpp"

class PlayerMove : public Tabby::ScriptableEntity {
private:
    Tabby::Entity childChildEntity;
    Tabby::TransformComponent* playerTransform;
    // Tabby::CircleCollider2DComponent* circleCollider;
    // Tabby::BoxCollider2DComponent* boxCollider;
    int facingDirection = 1;
    glm::vec2 input = { 0, 0 };
    glm::vec2 velocity;

    float playerSpeed = 2.5f;

public:
    PlayerMove()
    {
    }

    void OnCreate() override
    {
        playerTransform = &GetComponent<Tabby::TransformComponent>();
        // circleCollider = &GetComponent<Tabby::CircleCollider2DComponent>();

        // boxCollider = &Tabby::Entity(Tabby::Entity(GetComponent<Tabby::HierarchyNodeComponent>().Children[0].second).GetComponent<Tabby::HierarchyNodeComponent>().Children[0].second).GetComponent<Tabby::BoxCollider2DComponent>();

        // boxCollider = &Tabby::SceneManager::GetCurrentScene()->FindEntityByName("ChildChildEntity").GetComponent<Tabby::BoxCollider2DComponent>();

        // Tabby::Entity RigidbodyEntity = Tabby::SceneManager::GetCurrentScene()->CreateEntity("RigidbodyEntity2");
        // {
        //
        //     RigidbodyEntity.GetComponent<Tabby::TransformComponent>().Translation.y = -1.1f;
        //     auto& spriteComponent = RigidbodyEntity.AddComponent<Tabby::SpriteRendererComponent>();
        //
        //     // NOTE: Collider components have to be added before rigidbody component
        //     auto& boxColliderComponent = RigidbodyEntity.AddComponent<Tabby::BoxCollider2DComponent>();
        //     boxColliderComponent.Size = { 0.19f, 0.24f };
        //
        //     auto& rigidbodyComponent = RigidbodyEntity.AddComponent<Tabby::Rigidbody2DComponent>();
        //     rigidbodyComponent.Type = Tabby::Rigidbody2DComponent::BodyType::Dynamic;
        // }
    }

    void Update(Tabby::Timestep ts) override
    {

        CheckIfShouldFlip();

        if (Tabby::Input::IsKeyPressed(Tabby::Key::X)) {

            // Tabby::Entity RigidbodyEntity = Tabby::SceneManager::GetCurrentScene()->CreateEntity("RigidbodyEntity2");
            // {
            //
            //     RigidbodyEntity.GetComponent<Tabby::TransformComponent>().Translation.x = 0.25f;
            //     RigidbodyEntity.GetComponent<Tabby::TransformComponent>().Translation.y = 1.25f;
            //
            //     // NOTE: Collider components have to be added before rigidbody component
            //     auto& boxColliderComponent = RigidbodyEntity.AddComponent<Tabby::BoxCollider2DComponent>();
            //     boxColliderComponent.Size = { 0.19f, 0.24f };
            //
            //     auto& rigidbodyComponent = RigidbodyEntity.AddComponent<Tabby::Rigidbody2DComponent>();
            //     rigidbodyComponent.Type = Tabby::Rigidbody2DComponent::BodyType::Dynamic;
            // }

            Tabby::RaycastFilter2D filter;
            filter.SetCollisionLayer(0xFFFFFFFF); // Equal to setting layer to everything
            // filter.SetCollisionMask(0xFFFFFFFF); // Equal to setting layer to everything
            filter.SetMaskValue(1, false); // Enable 3rd layer to be hit to
            filter.SetMaskValue(2, false); // Enable 3rd layer to be hit to
            filter.SetMaskValue(3, true); // Enable 3rd layer to be hit to
            filter.SetMaskValue(4, true); // Enable 4th layer to be hit to

            Tabby::RaycastHit2D hit = Tabby::Physisc2D::RayCast(playerTransform->Translation + glm::vec3(0.0f, 1.0f, 0.0f), { 0.0, -1.0f }, 100.0f, filter, -10, 10);
            // Tabby::RaycastHit2D hit = Tabby::Physisc2D::BoxCast({ 0.2f, 0.2f }, playerTransform->Translation, { 0.0, -1.0f }, 100.0f, -10, 10);
            // Tabby::RaycastHit2D hit = Tabby::Physisc2D::CapsuleCast({ 0.0f, 0.2f }, { 0.0f, -0.2f }, 0.2f, playerTransform->Translation, { 0.0, -1.0f }, 100.0f, -10, 10);
            if (hit) {
                TB_INFO("Normal: {0} : {1}", hit.normal.x, hit.normal.y);
                TB_INFO("Point: {0} : {1}", hit.point.x, hit.point.y);
                TB_INFO("Velocity: {0} {1}", hit.rigidbody->GetVelocity().x, hit.rigidbody->GetVelocity().y);
                TB_INFO("Translation: {0} {1}", hit.transform->Translation.x, hit.transform->Translation.y);
                TB_INFO("Distance: {0}", hit.distance);
                TB_INFO("Fraction: {0}", hit.fraction);
                TB_INFO("Tag: {0}", hit.entity.GetComponent<Tabby::TagComponent>().Tag);
                Tabby::Debug::DrawLine({ hit.origin, 0 }, { hit.point, 0 });
            } else {
                TB_TRACE("IS NOT VALID");
            }
        }

        // float leftTriggerAxis = Tabby::Input::GetGamepadAxis(0, Tabby::Gamepad::GAMEPAD_AXIS_LEFT_TRIGGER);
        // float rightTriggerAxis = Tabby::Input::GetGamepadAxis(0, Tabby::Gamepad::GAMEPAD_AXIS_RIGHT_TRIGGER);
        //
        // bool GamepadButtonLeftLeft = Tabby::Input::IsGamepadButtonPressed(0, Tabby::Gamepad::GAMEPAD_BUTTON_LEFT_FACE_LEFT);
        // bool GamepadButtonLeftRight = Tabby::Input::IsGamepadButtonPressed(0, Tabby::Gamepad::GAMEPAD_BUTTON_LEFT_FACE_RIGHT);
        // bool GamepadButtonLeftUp = Tabby::Input::IsGamepadButtonPressed(0, Tabby::Gamepad::GAMEPAD_BUTTON_LEFT_FACE_UP);
        // bool GamepadButtonLeftDown = Tabby::Input::IsGamepadButtonPressed(0, Tabby::Gamepad::GAMEPAD_BUTTON_LEFT_FACE_DOWN);
        //
        // bool GamepadButtonRightLeft = Tabby::Input::IsGamepadButtonPressed(0, Tabby::Gamepad::GAMEPAD_BUTTON_RIGHT_FACE_LEFT);
        // bool GamepadButtonRightRight = Tabby::Input::IsGamepadButtonPressed(0, Tabby::Gamepad::GAMEPAD_BUTTON_RIGHT_FACE_RIGHT);
        // bool GamepadButtonRightUp = Tabby::Input::IsGamepadButtonPressed(0, Tabby::Gamepad::GAMEPAD_BUTTON_RIGHT_FACE_UP);
        // bool GamepadButtonRightDown = Tabby::Input::IsGamepadButtonPressed(0, Tabby::Gamepad::GAMEPAD_BUTTON_RIGHT_FACE_DOWN);
        //
        // TB_INFO("Left Left: {0}\n\tLeft Right: {1}\n\tLeft Up: {2}\n\tLeft Down: {3}\n\tRight Left: {4}\n\tRight Right: {5}\n\tRight Up: {6}\n\tRight Down: {7}" //
        //     ,
        //     GamepadButtonLeftLeft, GamepadButtonLeftRight, GamepadButtonLeftUp, GamepadButtonLeftDown //
        //     ,
        //     GamepadButtonRightLeft, GamepadButtonRightRight, GamepadButtonRightUp, GamepadButtonRightDown); //
        //
        // TB_INFO("Left Trigger Axis: {0}\n\tRight Trigger Axis: {1}", leftTriggerAxis, rightTriggerAxis);
    }

    // int lateUpdateCallTime = 0;
    // int fixedUpdateCallTime = 0;
    void LateUpdate(Tabby::Timestep ts) override
    {
        Move(ts);

        // TB_INFO("LateUpdate: {0} \n\t\tFixedUpdate: {1}", ++lateUpdateCallTime, fixedUpdateCallTime);
    }

    void FixedUpdate(Tabby::Timestep ts) override
    {
        // ++fixedUpdateCallTime;
        // TB_INFO("FixedUpdate: {0}", ++fixedUpdateCallTime);
    }

    void Draw() override
    {
        Tabby::Debug::DrawLine({ -2.0f, -2.0f, 0.0f }, { 2.0f, 2.0f, 0.0f });
    }

    void CheckIfShouldFlip()
    {
        if (input.x != 0 && input.x != facingDirection) {
            Flip();
        }
    }

    void Flip()
    {

        auto& playerTransform = GetComponent<Tabby::TransformComponent>();
        if (playerTransform.Rotation.y == 0) {

            playerTransform.Rotation.y = 180.0f;
        } else {
            playerTransform.Rotation.y = 0.0f;
        }
        facingDirection *= -1;
    }

    void Move(float dt)
    {
        input = { 0, 0 };

        auto& playerTransform = GetComponent<Tabby::TransformComponent>();
        if (Tabby::Input::IsKeyPressed(Tabby::Key::X)) {
            playerTransform.Rotation.y -= 10 * dt;
        }
        if (Tabby::Input::IsKeyPressed(Tabby::Key::Y)) {
            playerTransform.Rotation.y += 10 * dt;
        }

        if (Tabby::Input::IsKeyPressed(Tabby::Key::D)) {
            input.x = 1;
        }
        if (Tabby::Input::IsKeyPressed(Tabby::Key::A)) {
            input.x = -1;
        }

        if (Tabby::Input::IsKeyPressed(Tabby::Key::W)) {
            input.y = 1;
            if (Tabby::SceneManager::GetCurrentSceneName() != "Test")
                Tabby::SceneManager::SwitchTo("Test");
        }
        if (Tabby::Input::IsKeyPressed(Tabby::Key::S)) {
            input.y = -1;
            if (Tabby::SceneManager::GetCurrentSceneName() != "Test2")
                Tabby::SceneManager::SwitchTo("Test2");
        }
        if (Tabby::Input::IsKeyPressed(Tabby::Key::Q)) {
            input.y = -1;
            if (Tabby::SceneManager::GetCurrentSceneName() != "Test3")
                Tabby::SceneManager::SwitchTo("Test3");
        }

        // input.x = Tabby::Input::GetGamepadAxis(0, Tabby::Gamepad::Axis::GAMEPAD_AXIS_LEFT_X);
        // input.y = Tabby::Input::GetGamepadAxis(0, Tabby::Gamepad::Axis::GAMEPAD_AXIS_LEFT_Y);

        // TB_INFO("Input: ({0}, {1})", input.x, input.y);

        velocity = { input.x * playerSpeed, input.y * playerSpeed };

        auto& playerRB = GetComponent<Tabby::Rigidbody2DComponent>();
        if (&playerRB) {

            playerRB.SetVelocity({ velocity.x, playerRB.GetVelocity().y });

            // GetComponent<Tabby::Rigidbody2DComponent>().SetVelocity(velocity);

            if (Tabby::Input::IsKeyPressed(Tabby::Key::R)) {
                playerRB.SetAngularVelocity(2);
                // playerTransform.Rotation.z += 2;
            }
            if (Tabby::Input::IsKeyPressed(Tabby::Key::T)) {
                // playerTransform.Rotation.x += 2;
            }
            if (Tabby::Input::IsKeyPressed(Tabby::Key::Y)) {
                // playerTransform.Rotation.y += 2;
            }
        }

        // glm::vec2 trigger = { 0.0f, 0.0f };
        // if (Tabby::Input::IsGamepadButtonPressed(0, Tabby::Gamepad::Buttons::GAMEPAD_BUTTON_RIGHT_TRIGGER)) {
        //     trigger.y = 1;
        // }
        //
        // if (Tabby::Input::IsGamepadButtonPressed(0, Tabby::Gamepad::Buttons::GAMEPAD_BUTTON_LEFT_TRIGGER)) {
        //     trigger.x = 1;
        // }
        // TB_INFO("Gamepad Trigger: {0}, {1}", trigger.x, trigger.y);
        //
        // glm::vec2 shoulder = { 0.0f, 0.0f };
        // if (Tabby::Input::IsGamepadButtonPressed(0, Tabby::Gamepad::Buttons::GAMEPAD_BUTTON_RIGHT_SHOULDER)) {
        //     shoulder.y = 1;
        // }
        //
        // if (Tabby::Input::IsGamepadButtonPressed(0, Tabby::Gamepad::Buttons::GAMEPAD_BUTTON_LEFT_SHOULDER)) {
        //     shoulder.x = 1;
        // }
        // TB_INFO("Gamepad Shoulder: {0}, {1}", shoulder.x, shoulder.y);
        //
        // glm::vec2 leftFace = { 0.0f, 0.0f };
        // if (Tabby::Input::IsGamepadButtonPressed(0, Tabby::Gamepad::Buttons::GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) {
        //     leftFace.x = 1;
        // }
        //
        // if (Tabby::Input::IsGamepadButtonPressed(0, Tabby::Gamepad::Buttons::GAMEPAD_BUTTON_LEFT_FACE_LEFT)) {
        //     leftFace.x = -1;
        // }
        //
        // if (Tabby::Input::IsGamepadButtonPressed(0, Tabby::Gamepad::Buttons::GAMEPAD_BUTTON_LEFT_FACE_UP)) {
        //     leftFace.y = 1;
        // }
        //
        // if (Tabby::Input::IsGamepadButtonPressed(0, Tabby::Gamepad::Buttons::GAMEPAD_BUTTON_LEFT_FACE_DOWN)) {
        //     leftFace.y = -1;
        // }
        // TB_INFO("Gamepad Left Face: {0}, {1}", leftFace.x, leftFace.y);
        //
        // glm::vec2 rightFace = { 0.0f, 0.0f };
        // if (Tabby::Input::IsGamepadButtonPressed(0, Tabby::Gamepad::Buttons::GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)) {
        //     rightFace.x = 1;
        // }
        //
        // if (Tabby::Input::IsGamepadButtonPressed(0, Tabby::Gamepad::Buttons::GAMEPAD_BUTTON_RIGHT_FACE_LEFT)) {
        //     rightFace.x = -1;
        // }
        //
        // if (Tabby::Input::IsGamepadButtonPressed(0, Tabby::Gamepad::Buttons::GAMEPAD_BUTTON_RIGHT_FACE_UP)) {
        //     rightFace.y = 1;
        // }
        //
        // if (Tabby::Input::IsGamepadButtonPressed(0, Tabby::Gamepad::Buttons::GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
        //     rightFace.y = -1;
        // }
        // TB_INFO("Gamepad Right Face: {0}, {1}", rightFace.x, rightFace.y);
        //
        // glm::vec2 sticks = { 0.0f, 0.0f };
        // if (Tabby::Input::IsGamepadButtonPressed(0, Tabby::Gamepad::Buttons::GAMEPAD_BUTTON_LEFT_THUMB)) {
        //     sticks.x = 1;
        // }
        //
        // if (Tabby::Input::IsGamepadButtonPressed(0, Tabby::Gamepad::Buttons::GAMEPAD_BUTTON_RIGHT_THUMB)) {
        //     sticks.y = 1;
        // }
        // TB_INFO("Gamepad Sticks: {0}, {1}", sticks.x, sticks.y);
        //
        // glm::vec2 miscButton = { 0.0f, 0.0f };
        // if (Tabby::Input::IsGamepadButtonPressed(0, Tabby::Gamepad::Buttons::GAMEPAD_BUTTON_MIDDLE_LEFT)) {
        //     miscButton.x = 1;
        // }
        //
        // bool middelMiddle = false;
        // if (Tabby::Input::IsGamepadButtonPressed(0, Tabby::Gamepad::Buttons::GAMEPAD_BUTTON_MIDDLE)) {
        //     middelMiddle = true;
        // }
        //
        // if (Tabby::Input::IsGamepadButtonPressed(0, Tabby::Gamepad::Buttons::GAMEPAD_BUTTON_MIDDLE_RIGHT)) {
        //     miscButton.y = 1;
        // }
        // TB_INFO("Gamepad middle: {0}, {1}, {2}", miscButton.x, middelMiddle, miscButton.y);

        if (Tabby::Input::IsKeyPressed(Tabby::Key::Escape)) {
            Tabby::Application::Get().Close();
        }
    }

    void OnCollisionEnter(Tabby::ContactCallback contact) override
    {
        TB_INFO("COLLISION ENTER");

        TB_INFO("Velocity: {0} {1}", contact.rigidbody->GetVelocity().x, contact.rigidbody->GetVelocity().y);
        TB_INFO("Translation: {0} {1}\n", contact.transform->Translation.x, contact.transform->Translation.y);
    }
    void OnCollisionExit(Tabby::ContactCallback contact) override
    {
        TB_INFO("COLLISION ExIT");

        TB_INFO("Velocity: {0} {1}", contact.rigidbody->GetVelocity().x, contact.rigidbody->GetVelocity().y);
        TB_INFO("Translation: {0} {1}\n", contact.transform->Translation.x, contact.transform->Translation.y);
    }
    void OnPreSolve(Tabby::ContactCallback contact) override { }
    void OnPostSolve(Tabby::ContactCallback contact) override { }
};
