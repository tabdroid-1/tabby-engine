#include <Tabby.h>

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

    Tabby::Entity TextEntity = CreateEntity("TextEntity");
    {
        auto& textComponent = TextEntity.AddComponent<Tabby::TextComponent>();
        textComponent.TextString = "Successfull text rendering!!";
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
