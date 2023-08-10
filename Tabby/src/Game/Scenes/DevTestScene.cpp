#include "DevTestScene.h"
#include "ComponentSystem/Components/C_Rigidbody.h"
#include "Scene/GameObject.h"
#include "Scene/SceneStateMachine.h"
#include "box2d/b2_body.h"
#include "box2d/b2_world.h"
#include "raylib.h"
#include <Util/DebugUtils.h>
#include <cstddef>
#include <memory>

std::shared_ptr<GameObject> playerCamera = std::make_shared<GameObject>("playerCamera");

DevTestScene::DevTestScene(SceneStateMachine& sceneStateMachine)
    : sceneStateMachine(sceneStateMachine)
{
}

void DevTestScene::OnCreate()
{
    physics.Load({ 0.0f, 30.81f });

    DisableCursor();

    // --------- Camera ---------
    // auto cameraMove = playerCamera->AddComponent<PlayerMove>();
    // cameraMove->Load(200.0f);
    auto camera = playerCamera->AddComponent<C_Camera>();
    camera->Load({ 20.0f, 20.0f });

    sceneCameras.insert(std::pair<std::string, Camera2D>(playerCamera->GetName(), camera->GetCamera()));

    gameObjects.Add(playerCamera);

    // --------- Ground ---------

    std::shared_ptr<GameObject> ground = std::make_shared<GameObject>("ground");
    ground->transform->SetPosition(1250, 1200);

    auto groundRigidbody = ground->AddComponent<C_Rigidbody>();
    groundRigidbody->Load(physics.GetPhysicsWorld(), b2_staticBody, physics.GetPhysicsWorldScale());
    groundRigidbody->CreateBoxFixture({ 50.0f, 1.0f }, 1.0f, 1.0f);
    gameObjects.Add(ground);

    // --------- Tilemap ---------
    // std::shared_ptr<GameObject> tilemap = std::make_shared<GameObject>("tilemap");
    // auto tilemapComponent = tilemap->AddComponent<C_LDtkTilemap>();
    // tilemap->transform->SetPosition({ 8.0f, 8.0f });
    // tilemap->transform->SetScale({ 2.0f, 2.0f });
    //
    // tilemapComponent->Load("assets/tilemaps/world.ldtk", "Level_0");
    // // tilemapComponent->Load("assets/tilemaps/desert.tmx");
    //
    // tilemap->GetDrawable()->SetSortOrder(-1);
    // gameObjects.Add(tilemap);

    // --------- Player ---------
}

void DevTestScene::OnActivate()
{
    auto& playerObjcet = gameObjects.GetGameObject("player");
    auto playerRigidbody = playerObjcet.AddComponent<C_Rigidbody>();
    playerRigidbody->Load(physics.GetPhysicsWorld(), b2_dynamicBody, true, physics.GetPhysicsWorldScale());
    playerRigidbody->CreateCapsuleFixture({ 1.0f, 2.4f }, { 0.0f, -0.15f }, 1.0f, 1.0f);

    auto playeMoveScript = playerObjcet.AddComponent<PlayerMove>();
    playeMoveScript->Load(10);
}

void DevTestScene::Update(float dt)
{

    gameObjects.ProcessRemovals();
    gameObjects.ProcessNewObjects();
    gameObjects.Update(dt);

    playerCamera->GetComponent<C_Camera>()->UpdateCamera(sceneCameras, playerCamera->GetName());
    activeCamera = sceneCameras.find(playerCamera->GetName())->second;

    auto& playerObject = gameObjects.GetGameObject("player");
    if (IsKeyDown(KEY_D)) {
        playerObject.GetComponent<C_Rigidbody>()->AddVelocityX(2);
    } else if (IsKeyDown(KEY_A)) {
        playerObject.GetComponent<C_Rigidbody>()->AddVelocityX(-2);
    } else if (IsKeyDown(KEY_R)) {
        playerObject.GetComponent<C_Rigidbody>()->AddAngularVelocity(1);
    } else if (IsKeyDown(KEY_C)) {
        playerObject.transform->SetPosition({ 500.0f, 400.f });
    }
}

void DevTestScene::LateUpdate(float dt)
{

    physics.Update(dt);
    gameObjects.LateUpdate(dt);
}

void DevTestScene::Draw()
{

    BeginMode2D(activeCamera);
    gameObjects.Draw();
    physics.Draw(activeCamera);
    EndMode2D();

    // Debug

    DrawFPS(0, 0);
    DrawText("Use dasd balls gunh hmm ahh yes com here", 0, 20, 20, GRAY);

    // DrawText(TextFormat("- Position: (%06.3f, %06.3f, %06.3f)", position.x, position.y, position.z), 0, 40, 20, GRAY);
}

void DevTestScene::OnDestroy()
{
    physics.Destroy();
}
