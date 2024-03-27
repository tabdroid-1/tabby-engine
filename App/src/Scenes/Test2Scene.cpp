#include <Scripts/PlayerScript.h>
#include <Tabby.h>

#include "Tabby/Core/Base.h"
#include "Tabby/Scene/Components.h"
#include "Test2Scene.h"
#include "imgui/imgui.h"

Test2Scene::Test2Scene()
{
}

void Test2Scene::OnCreate()
{
}

void Test2Scene::OnActivate()
{
    Tabby::Entity CameraEntity = CreateEntity("CameraEntity");
    {
        auto& cameraComponent = CameraEntity.AddComponent<Tabby::CameraComponent>();
        cameraComponent.Primary = true;
    }

    Tabby::Entity TextEntity = CreateEntity("TextEntity");
    {
        auto& textComponent = TextEntity.AddComponent<Tabby::TextComponent>();
        textComponent.TextString = "Test Scene 2";
    }
}

void Test2Scene::DrawImGui()
{
}
