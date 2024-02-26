#include <Scripts/PlayerScript.h>
#include <Tabby.h>

#include "Tabby/Core/Base.h"
#include "Tabby/Scene/Components.h"
#include "Tabby/Sound/SoundBuffer.h"
#include "TestScene.h"
#include "imgui/imgui.h"

TestScene::TestScene()
{
}

void TestScene::OnCreate()
{
}

void TestScene::OnActivate()
{

    Tabby::Entity RigidbodyEntity = CreateEntity("RigidbodyEntity2");
    {

        auto& idC = RigidbodyEntity.GetComponent<Tabby::IDComponent>();
        idC.IsPersistent = true;

        RigidbodyEntity.GetComponent<Tabby::TransformComponent>().Translation.x = 0.25f;
        RigidbodyEntity.GetComponent<Tabby::TransformComponent>().Translation.y = 5.25f;

        auto& boxColliderComponent = RigidbodyEntity.AddComponent<Tabby::BoxCollider2DComponent>();
        boxColliderComponent.Size = { 0.19f, 0.24f };

        auto& rigidbodyComponent = RigidbodyEntity.AddComponent<Tabby::Rigidbody2DComponent>();
        rigidbodyComponent.Type = Tabby::Rigidbody2DComponent::BodyType::Dynamic;
    }

    Tabby::Entity GroundEntity = CreateEntity("RigidbodyEntity");
    {
        auto& idC = GroundEntity.GetComponent<Tabby::IDComponent>();
        idC.IsPersistent = true;

        GroundEntity.GetComponent<Tabby::TransformComponent>().Translation.y = -2;

        auto& boxColliderComponent = GroundEntity.AddComponent<Tabby::BoxCollider2DComponent>();
        boxColliderComponent.Size = { 4.0f, 0.5f };

        auto& rigidbodyComponent = GroundEntity.AddComponent<Tabby::Rigidbody2DComponent>();
        rigidbodyComponent.Type = Tabby::Rigidbody2DComponent::BodyType::Static;
    }

    Tabby::Entity CameraEntity = CreateEntity("CameraEntity");
    {
        auto& cameraComponent = CameraEntity.AddComponent<Tabby::CameraComponent>();
        cameraComponent.Primary = true;
    }

    // Tabby::Entity TextEntity2 = CreateEntity("Text2Entity");
    // {
    //     TextEntity2.GetComponent<Tabby::TransformComponent>().Translation.y = -2;
    //     auto& textComponent = TextEntity2.AddComponent<Tabby::TextComponent>();
    //     textComponent.TextString = "ComicSans at home";
    //     textComponent.FontAsset = Tabby::CreateRef<Tabby::Font>("assets/fonts/ldfcomicsans/Ldfcomicsans-jj7l.ttf");
    // }

    Tabby::Entity TextEntity = CreateEntity("TextEntity");
    {
        auto& textComponent = TextEntity.AddComponent<Tabby::TextComponent>();
        textComponent.TextString = "Opensans";
    }

    Tabby::Entity ChildEntity = CreateEntity("ChildEntity");
    {
        auto& spriteComponent = ChildEntity.AddComponent<Tabby::SpriteRendererComponent>();
    }

    Tabby::Entity SpriteEntity = CreateEntity("SpriteEntity");
    {
        auto& idC = SpriteEntity.GetComponent<Tabby::IDComponent>();
        idC.IsPersistent = true;

        SpriteEntity.AddChild(ChildEntity);

        SpriteEntity.GetComponent<Tabby::TransformComponent>().Translation.y = 3.9f;
        auto& spriteComponent = SpriteEntity.AddComponent<Tabby::SpriteRendererComponent>();
        spriteComponent.Texture = Tabby::Texture2D::Create("assets/spritesheets/player/player_packed.png");
        spriteComponent.renderOrder = 1;
        spriteComponent.hFrames = 10;
        spriteComponent.vFrames = 5;
        spriteComponent.xFrame = 2;
        spriteComponent.yFrame = 4;

        // auto& soundComponent = SpriteEntity.AddComponent<Tabby::SoundComponent>();
        // soundComponent.Sound = Tabby::CreateRef<Tabby::SoundBuffer>("assets/audio/sunflower-street.mp3");
        // soundComponent.Playing = true;

        auto& boxColliderComponent = SpriteEntity.AddComponent<Tabby::BoxCollider2DComponent>();
        boxColliderComponent.Size = { 0.19f, 0.24f };

        auto& rigidbodyComponent = SpriteEntity.AddComponent<Tabby::Rigidbody2DComponent>();
        rigidbodyComponent.Type = Tabby::Rigidbody2DComponent::BodyType::Dynamic;

        auto& playerMovement = SpriteEntity.AddComponent<Tabby::NativeScriptComponent>();
        playerMovement.Bind<PlayerMove>();
        auto playerMovementScript = static_cast<PlayerMove*>(playerMovement.Instance);
    }
}

void TestScene::DrawImGui()
{
}
