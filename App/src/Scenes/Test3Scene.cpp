#include <Scripts/PlayerScript.h>
#include <Tabby.h>

#include "Tabby/Core/Base.h"
#include "Tabby/Scene/Components.h"
// #include "Tabby/Sound/SoundBuffer.h"
#include "Test3Scene.h"

Test3Scene::Test3Scene()
{
}

void Test3Scene::OnCreate()
{
}

void Test3Scene::OnActivate()
{
    Tabby::Entity CameraEntity = CreateEntity("CameraEntity");
    {
        auto& cameraComponent = CameraEntity.AddComponent<Tabby::CameraComponent>();
        cameraComponent.Primary = true;
    }

    Tabby::Entity TextEntity = CreateEntity("TextEntity");
    {
        auto& textComponent = TextEntity.AddComponent<Tabby::TextComponent>();
        textComponent.TextString = "Test Scene 3";
    }
}

void Test3Scene::DrawImGui()
{
}
