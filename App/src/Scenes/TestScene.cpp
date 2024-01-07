#include <Tabby.h>

#include "Tabby/Core/Base.h"
#include "Tabby/Scene/Components.h"
#include "TestScene.h"

TestScene::TestScene()
{
}

void TestScene::OnCreate()
{
}

void TestScene::OnActivate()
{

    Tabby::Entity CameraEntity = CreateEntity("CameraEntity");
    {
        auto& cameraComponent = CameraEntity.AddComponent<Tabby::CameraComponent>();
        cameraComponent.Primary = true;
    }

    Tabby::Entity TextEntity2 = CreateEntity("Text2Entity");
    {
        TextEntity2.GetComponent<Tabby::TransformComponent>().Translation.y = -2;
        auto& textComponent = TextEntity2.AddComponent<Tabby::TextComponent>();
        textComponent.TextString = "ComicSans at home";
        textComponent.FontAsset = Tabby::CreateRef<Tabby::Font>("assets/fonts/ldfcomicsans/Ldfcomicsans-jj7l.ttf");
    }

    Tabby::Entity TextEntity = CreateEntity("TextEntity");
    {
        auto& textComponent = TextEntity.AddComponent<Tabby::TextComponent>();
        textComponent.TextString = "Opensans";
    }

    Tabby::Entity SpriteEntity = CreateEntity("SpriteEntity");
    {
        auto& spriteComponent = SpriteEntity.AddComponent<Tabby::SpriteRendererComponent>();
        spriteComponent.Texture = Tabby::Texture2D::Create("assets/textures/Tabby.png");
    }
}

void TestScene::DrawImGui()
{
}
