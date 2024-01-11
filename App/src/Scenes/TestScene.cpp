#include <Tabby.h>

#include "Tabby/Core/Base.h"
#include "Tabby/Scene/Components.h"
#include "TestScene.h"
#include "imgui/imgui.h"

Tabby::Ref<Tabby::Texture2D> texture = nullptr;

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

        RigidbodyEntity.GetComponent<Tabby::TransformComponent>().Translation.x = 0.25f;
        RigidbodyEntity.GetComponent<Tabby::TransformComponent>().Translation.y = 5.25f;
        auto& rigidbodyComponent = RigidbodyEntity.AddComponent<Tabby::Rigidbody2DComponent>();
        rigidbodyComponent.Type = Tabby::Rigidbody2DComponent::BodyType::Dynamic;

        auto& boxColliderComponent = RigidbodyEntity.AddComponent<Tabby::BoxCollider2DComponent>();
        boxColliderComponent.Size = { 0.19f, 0.24f };
    }

    Tabby::Entity GroundEntity = CreateEntity("RigidbodyEntity");
    {
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
        SpriteEntity.AddChild(ChildEntity);

        SpriteEntity.GetComponent<Tabby::TransformComponent>().Translation.y = 5.9f;
        auto& spriteComponent = SpriteEntity.AddComponent<Tabby::SpriteRendererComponent>();
        // texture = Tabby::Texture2D::Create("assets/textures/Alive.png");
        texture = Tabby::Texture2D::Create("assets/spritesheets/player/player_packed.png");
        spriteComponent.Texture = texture;
        spriteComponent.hFrames = 10;
        spriteComponent.vFrames = 5;
        spriteComponent.xFrame = 2;
        spriteComponent.yFrame = 4;

        // auto& rigidbodyComponent = SpriteEntity.AddComponent<Tabby::Rigidbody2DComponent>();
        // rigidbodyComponent.Type = Tabby::Rigidbody2DComponent::BodyType::Dynamic;
        //
        // auto& boxColliderComponent = SpriteEntity.AddComponent<Tabby::BoxCollider2DComponent>();
        // boxColliderComponent.Size = { 0.19f, 0.24f };
    }
}

void TestScene::DrawImGui()
{
}
