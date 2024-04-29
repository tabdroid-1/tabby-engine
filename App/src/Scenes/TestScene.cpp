#include <Scripts/PlayerScript.h>
#include <Tabby.h>

#include "Tabby/Audio/AudioEngine.h"
#include "Tabby/Core/Base.h"
#include "Tabby/Scene/Components.h"
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

        auto& rigidbodyComponent = RigidbodyEntity.AddComponent<Tabby::Rigidbody2DComponent>();
        rigidbodyComponent.Type = Tabby::Rigidbody2DComponent::BodyType::Dynamic;

        auto& boxColliderComponent = RigidbodyEntity.AddComponent<Tabby::BoxCollider2DComponent>();
        boxColliderComponent.Size = { 0.19f, 0.24f };
        boxColliderComponent.enableContactEvents = true;
        boxColliderComponent.enableSensorEvents = true;
    }

    Tabby::Entity CapsuleRigidbodyEntity = CreateEntity("RigidbodyEntity2");
    {
        auto& idC = CapsuleRigidbodyEntity.GetComponent<Tabby::IDComponent>();
        idC.IsPersistent = true;

        CapsuleRigidbodyEntity.GetComponent<Tabby::TransformComponent>().Translation.x = 0.25f;
        CapsuleRigidbodyEntity.GetComponent<Tabby::TransformComponent>().Translation.y = 5.25f;

        auto& rigidbodyComponent = CapsuleRigidbodyEntity.AddComponent<Tabby::Rigidbody2DComponent>();
        rigidbodyComponent.Type = Tabby::Rigidbody2DComponent::BodyType::Dynamic;

        auto& capsuleColliderComponent = CapsuleRigidbodyEntity.AddComponent<Tabby::CapsuleCollider2DComponent>();
        capsuleColliderComponent.center1 = { 0, 0.5f };
        capsuleColliderComponent.center2 = { 0, -0.5f };
        capsuleColliderComponent.Radius = 0.4f;
    }

    Tabby::Entity GroundEntity = CreateEntity("RigidbodyEntity");
    {
        auto& idC = GroundEntity.GetComponent<Tabby::IDComponent>();
        idC.IsPersistent = true;

        GroundEntity.GetComponent<Tabby::TransformComponent>().Translation.y = -2;

        auto& rigidbodyComponent = GroundEntity.AddComponent<Tabby::Rigidbody2DComponent>();
        rigidbodyComponent.Type = Tabby::Rigidbody2DComponent::BodyType::Static;

        auto& boxColliderComponent = GroundEntity.AddComponent<Tabby::BoxCollider2DComponent>();
        boxColliderComponent.Size = { 4.0f, 0.5f };
    }

    Tabby::Entity CameraEntity = CreateEntity("CameraEntity");
    {
        auto& cameraComponent = CameraEntity.AddComponent<Tabby::CameraComponent>();
        cameraComponent.Primary = true;
    }

    Tabby::Entity TextEntity2 = CreateEntity("Text2Entity");
    {
        TextEntity2.GetComponent<Tabby::TransformComponent>().Translation.y = 2;
        auto& textComponent = TextEntity2.AddComponent<Tabby::TextComponent>();
        textComponent.TextString = "ComicSans at home";
        textComponent.font = Tabby::CreateShared<Tabby::Font>("fonts/ldfcomicsans/Ldfcomicsans-jj7l.ttf");
    }

    Tabby::Entity TextEntity = CreateEntity("TextEntity");
    {
        TextEntity.GetComponent<Tabby::TransformComponent>().Translation.y = 3;
        auto& textComponent = TextEntity.AddComponent<Tabby::TextComponent>();
        textComponent.TextString = "Opensans";
    }

    Tabby::Entity ChildChildEntity = CreateEntity("ChildChildEntity");
    {
        auto& spriteComponent = ChildChildEntity.AddComponent<Tabby::SpriteRendererComponent>();

        ChildChildEntity.GetComponent<Tabby::TransformComponent>().LocalTranslation.x = 0.55f;

        auto& boxColliderComponent = ChildChildEntity.AddComponent<Tabby::BoxCollider2DComponent>();
        boxColliderComponent.Size = { 0.19f, 0.24f };
        boxColliderComponent.isSensor = false;
        boxColliderComponent.enableContactEvents = true;
        boxColliderComponent.enableSensorEvents = true;
    }

    Tabby::Entity ChildEntity = CreateEntity("ChildEntity");
    {
        auto& spriteComponent = ChildEntity.AddComponent<Tabby::SpriteRendererComponent>();

        ChildEntity.AddChild(ChildChildEntity);
    }

    Tabby::Entity SpriteEntity = CreateEntity("SpriteEntity");
    {

        auto& idC = SpriteEntity.GetComponent<Tabby::IDComponent>();
        idC.IsPersistent = true;

        SpriteEntity.AddChild(ChildEntity);

        SpriteEntity.GetComponent<Tabby::TransformComponent>().Translation.y = 3.9f;
        auto& spriteComponent = SpriteEntity.AddComponent<Tabby::SpriteRendererComponent>();
        spriteComponent.Texture = Tabby::AssetManager::Get()->LoadAssetSource("spritesheets/player/player_packed.png", spriteComponent.Texture);
        spriteComponent.renderOrder = 1;
        spriteComponent.hFrames = 10;
        spriteComponent.vFrames = 5;
        spriteComponent.xFrame = 2;
        spriteComponent.yFrame = 4;

        auto& soundComponent = SpriteEntity.AddComponent<Tabby::SoundComponent>();

        // Stream Audio
        Tabby::Audio::Engine::SetPlayerMusic("audio/sunflower-street.wav");
        Tabby::Audio::Engine::PlayMusicPlayer();

        // // Load whole audio in to a buffer
        // Tabby::Audio::Engine::PlaySfx("music/sunflower-street.wav");

        soundComponent.Playing = true;

        auto& rigidbodyComponent = SpriteEntity.AddComponent<Tabby::Rigidbody2DComponent>();
        rigidbodyComponent.Type = Tabby::Rigidbody2DComponent::BodyType::Dynamic;

        // auto& boxColliderComponent = SpriteEntity.AddComponent<Tabby::BoxCollider2DComponent>();
        // boxColliderComponent.Size = { 0.19f, 0.24f };

        auto& circleColliderComponent = SpriteEntity.AddComponent<Tabby::CircleCollider2DComponent>();
        circleColliderComponent.Offset = { 0.5f, 0.5f };
        circleColliderComponent.Radius = 0.24f;

        auto& playerMovement = SpriteEntity.AddComponent<Tabby::NativeScriptComponent>();
        playerMovement.Bind<PlayerMove>();
        // auto playerMovementScript = static_cast<PlayerMove*>(playerMovement.Instance);
    }

    Tabby::Entity SpriteEntity2 = CreateEntity("SpriteEntity2");
    {
        auto& spriteComponent = SpriteEntity2.AddComponent<Tabby::SpriteRendererComponent>();
        spriteComponent.Texture = Tabby::AssetManager::Get()->LoadAssetSource("spritesheets/player/player_air_spin.png", spriteComponent.Texture);
    }

    Tabby::Entity SpriteEntity3 = CreateEntity("SpriteEntity3");
    {
        auto& spriteComponent = SpriteEntity3.AddComponent<Tabby::SpriteRendererComponent>();
        spriteComponent.Texture = Tabby::AssetManager::Get()->LoadAssetSource("spritesheets/player/player_crouch_idle.png", spriteComponent.Texture);
    }

    Tabby::Entity SpriteEntity4 = CreateEntity("SpriteEntity4");
    {
        auto& spriteComponent = SpriteEntity4.AddComponent<Tabby::SpriteRendererComponent>();
        spriteComponent.Texture = Tabby::AssetManager::Get()->LoadAssetSource("spritesheets/player/player_death.png", spriteComponent.Texture);
    }

    Tabby::Entity SpriteEntity5 = CreateEntity("SpriteEntity5");
    {
        auto& spriteComponent = SpriteEntity5.AddComponent<Tabby::SpriteRendererComponent>();
        spriteComponent.Texture = Tabby::AssetManager::Get()->LoadAssetSource("textures/NoLife.png", spriteComponent.Texture);
    }

    Tabby::Entity SpriteEntity6 = CreateEntity("SpriteEntity6");
    {
        auto& spriteComponent = SpriteEntity6.AddComponent<Tabby::SpriteRendererComponent>();
        spriteComponent.Texture = Tabby::AssetManager::Get()->LoadAssetSource("spritesheets/player/player_hurt.png", spriteComponent.Texture);
    }

    Tabby::Entity SpriteEntity7 = CreateEntity("SpriteEntity7");
    {
        auto& spriteComponent = SpriteEntity7.AddComponent<Tabby::SpriteRendererComponent>();
        spriteComponent.Texture = Tabby::AssetManager::Get()->LoadAssetSource("hello.png", spriteComponent.Texture);
    }

    Tabby::Entity SpriteEntity8 = CreateEntity("SpriteEntity8");
    {
        auto& spriteComponent = SpriteEntity8.AddComponent<Tabby::SpriteRendererComponent>();
        spriteComponent.Texture = Tabby::AssetManager::Get()->LoadAssetSource("textures/Alive.png", spriteComponent.Texture);
    }

    Tabby::Entity SpriteEntity9 = CreateEntity("SpriteEntity9");
    {
        auto& spriteComponent = SpriteEntity9.AddComponent<Tabby::SpriteRendererComponent>();
        spriteComponent.Texture = Tabby::AssetManager::Get()->LoadAssetSource("textures/crate.jpg", spriteComponent.Texture);
    }

    Tabby::Entity SpriteEntity10 = CreateEntity("SpriteEntity10");
    {
        auto& spriteComponent = SpriteEntity10.AddComponent<Tabby::SpriteRendererComponent>();
        spriteComponent.Texture = Tabby::AssetManager::Get()->LoadAssetSource("textures/Dead.png", spriteComponent.Texture);
    }

    Tabby::Entity SpriteEntity11 = CreateEntity("SpriteEntity11");
    {
        auto& spriteComponent = SpriteEntity11.AddComponent<Tabby::SpriteRendererComponent>();
        spriteComponent.Texture = Tabby::AssetManager::Get()->LoadAssetSource("textures/noTexture.png", spriteComponent.Texture);
    }

    Tabby::Entity SpriteEntity12 = CreateEntity("SpriteEntity12");
    {
        auto& spriteComponent = SpriteEntity12.AddComponent<Tabby::SpriteRendererComponent>();
        spriteComponent.Texture = Tabby::AssetManager::Get()->LoadAssetSource("tilemaps/Terrain.png", spriteComponent.Texture);
    }

    Tabby::Entity SpriteEntity13 = CreateEntity("SpriteEntity13");
    {
        auto& spriteComponent = SpriteEntity13.AddComponent<Tabby::SpriteRendererComponent>();
        spriteComponent.Texture = Tabby::AssetManager::Get()->LoadAssetSource("tilemaps/tmw_desert_spacing.png", spriteComponent.Texture);
    }

    Tabby::Entity SpriteEntity14 = CreateEntity("SpriteEntity14");
    {
        auto& spriteComponent = SpriteEntity14.AddComponent<Tabby::SpriteRendererComponent>();
        spriteComponent.Texture = Tabby::AssetManager::Get()->LoadAssetSource("tilemaps/topdown.png", spriteComponent.Texture);
    }

    Tabby::Entity SpriteEntity15 = CreateEntity("SpriteEntity15");
    {
        auto& spriteComponent = SpriteEntity15.AddComponent<Tabby::SpriteRendererComponent>();
        spriteComponent.Texture = Tabby::AssetManager::Get()->LoadAssetSource("random/dinoCharactersVersion1.1/sheets/DinoSprites - doux.png", spriteComponent.Texture);
    }

    Tabby::Entity SpriteEntity16 = CreateEntity("SpriteEntity16");
    {
        auto& spriteComponent = SpriteEntity16.AddComponent<Tabby::SpriteRendererComponent>();
        spriteComponent.Texture = Tabby::AssetManager::Get()->LoadAssetSource("random/dinoCharactersVersion1.1/sheets/DinoSprites - mort.png", spriteComponent.Texture);
    }

    Tabby::Entity SpriteEntity17 = CreateEntity("SpriteEntity17");
    {
        auto& spriteComponent = SpriteEntity17.AddComponent<Tabby::SpriteRendererComponent>();
        spriteComponent.Texture = Tabby::AssetManager::Get()->LoadAssetSource("random/dinoCharactersVersion1.1/sheets/DinoSprites - tard.png", spriteComponent.Texture);
    }

    // Tabby::SceneManager::GetCurrentScene()->DestroyEntityWithChildren(ChildEntity);
}

void TestScene::DrawImGui()
{
}
