#include "Scene/Components.h"
#include "Scene/GameObject.h"
#include "Scene/Scene.h"
#include "raylib.h"
#include <Game/Scenes/TitleScreenScene.h>
#include <Game/Scripts/Scripts.h>
#include <Tabby.h>

TitleScreenScene::TitleScreenScene(SceneStateMachine& sceneStateMachine)
    : sceneStateMachine(sceneStateMachine)
    , switchToState(0)
    , currentSeconds(0.f)
    , showForSeconds(1.f)
{
}

void TitleScreenScene::OnCreate()
{
}

void TitleScreenScene::OnActivate()
{
    m_Panel.SetContext(*this);
    DisableCursor();
    // ---------- Tabby -----------
    Tabby::GameObject tabbySpriteHolder = CreateEntity("tabbySprite");
    tabbySpriteHolder.GetComponent<Tabby::TransformComponent>().localPosition = { 0.0f, 0.0f, -1.0f };

    auto& tabbySprite = tabbySpriteHolder.AddComponent<Tabby::SpriteRendererComponent>();
    /* playerSprite.Texture = LoadTexture("assets/spritesheets/player/player_idle.png"); */
    tabbySprite.SetTexture("assets/random/tabby.png");

    // ---------- Player ----------
    Tabby::GameObject player = CreateEntity("player");
    player.GetComponent<Tabby::TransformComponent>().position = {
        0.0f,
        3.0f,
        1.0f,
    };

    player.AddChild(tabbySpriteHolder);

    /* player.GetComponent<Tabby::TransformComponent>().Size = { 48.0f, 48.0f }; */
    player.GetComponent<Tabby::TransformComponent>().scale = { 1.25f, 1.25f };

    auto& playerSprite = player.AddComponent<Tabby::SpriteRendererComponent>();
    /* playerSprite.Texture = LoadTexture("assets/spritesheets/player/player_idle.png"); */
    playerSprite.SetTexture("assets/spritesheets/player/player_idle.png");

    auto& playerRigidbody = player.AddComponent<Tabby::RigidBodyComponent>();
    playerRigidbody.Type = Tabby::RigidBodyComponent::BodyType::Dynamic;
    playerRigidbody.FixedRotation = true;
    auto& playerCapsuleCollider = player.AddComponent<Tabby::BoxCollider2DComponent>();
    playerCapsuleCollider.Size = { 0.2f, 0.50f };
    playerCapsuleCollider.Friction = 1;
    playerCapsuleCollider.Density = 1;
    playerCapsuleCollider.Bounce = 0;
    playerCapsuleCollider.BounceThreshold = 0;

    auto& playerAnimator = player.AddComponent<Tabby::AnimationComponent>();
    std::shared_ptr<Animation> idleAnimation = std::make_shared<Animation>();

    const int frameWidth = 48;
    const int frameHeight = 48;

    const float idleAnimFrameSeconds = 0.2f;
    idleAnimation->AddFrame(0, 0, frameWidth, frameHeight, idleAnimFrameSeconds, "assets/spritesheets/player/player_idle.png");
    idleAnimation->AddFrame(48, 0, frameWidth, frameHeight, idleAnimFrameSeconds, "assets/spritesheets/player/player_idle.png");
    idleAnimation->AddFrame(96, 0, frameWidth, frameHeight, idleAnimFrameSeconds, "assets/spritesheets/player/player_idle.png");
    idleAnimation->AddFrame(144, 0, frameWidth, frameHeight, idleAnimFrameSeconds, "assets/spritesheets/player/player_idle.png");
    idleAnimation->AddFrame(192, 0, frameWidth, frameHeight, idleAnimFrameSeconds, "assets/spritesheets/player/player_idle.png");
    idleAnimation->AddFrame(240, 0, frameWidth, frameHeight, idleAnimFrameSeconds, "assets/spritesheets/player/player_idle.png");
    idleAnimation->AddFrame(288, 0, frameWidth, frameHeight, idleAnimFrameSeconds, "assets/spritesheets/player/player_idle.png");
    idleAnimation->AddFrame(336, 0, frameWidth, frameHeight, idleAnimFrameSeconds, "assets/spritesheets/player/player_idle.png");
    idleAnimation->AddFrame(384, 0, frameWidth, frameHeight, idleAnimFrameSeconds, "assets/spritesheets/player/player_idle.png");
    idleAnimation->AddFrame(432, 0, frameWidth, frameHeight, idleAnimFrameSeconds, "assets/spritesheets/player/player_idle.png");
    playerAnimator.AddAnimation("idle", idleAnimation);
    playerAnimator.PlayAnimation("idle");

    auto& playerMovement = player.AddComponent<Tabby::NativeScriptComponent>();
    playerMovement.Bind<PlayerMove>();
    auto playerMovementScript = static_cast<PlayerMove*>(playerMovement.Instance);

    // ---------- Camera ----------
    Tabby::GameObject cameraObject = CreateEntity("mainCamera");
    auto& cameraPosition = cameraObject.GetComponent<Tabby::TransformComponent>().position;
    cameraPosition = { 0.0f, 2.0f, 10.0f };
    auto& camera = cameraObject.AddComponent<Tabby::CameraComponent>();
    camera.camera.target = { cameraPosition.x, cameraPosition.y, cameraPosition.z - 1 };
    camera.isMainCamera = true;
    camera.camera.projection = CAMERA_ORTHOGRAPHIC;
    camera.camera.fovy = 10.0f;

    auto& cameraMovement = cameraObject.AddComponent<Tabby::NativeScriptComponent>();
    cameraMovement.Bind<CameraMove>();
    auto cameraMovementScript = static_cast<CameraMove*>(cameraMovement.Instance);

    // ---------- Ground ----------
    Tabby::GameObject ground = CreateEntity("ground");
    ground.GetComponent<Tabby::TransformComponent>().position = { 3.0f, -1.0f };
    auto& groundRigidbody = ground.AddComponent<Tabby::RigidBodyComponent>();
    groundRigidbody.Type = Tabby::RigidBodyComponent::BodyType::Static;

    auto& groundBoxCollider = ground.AddComponent<Tabby::BoxCollider2DComponent>();
    groundBoxCollider.Size = { 50.0f, 1.0f };
}

void TitleScreenScene::DrawImGui()
{
    // bool show = true;
    // ImGui::ShowDemoWindow(&show);

    Tabby::GameObject selectedentity = m_Panel.GetSelectedEntity();
    m_Panel.OnImGuiRender();
}

void TitleScreenScene::OnDestroy()
{
}

// #include <memory>
//
// TitleScreenScene::TitleScreenScene(SceneStateMachine& sceneStateMachine)
//     : sceneStateMachine(sceneStateMachine)
//     , switchToState(0)
//     , currentSeconds(0.f)
//     , showForSeconds(1.f)
// {
// }
//
// void TitleScreenScene::OnCreate()
// {
//
//     // ----------- Player -----------
//     std::shared_ptr<GameObject> player = std::make_shared<GameObject>("player", true);
//     player->transform->SetPosition({ 500.0f, 400.f });
//     player->transform->SetSize({ 48.0f, 48.f });
//     player->transform->SetScale({ 4.0f, 4.f });
//
//     auto sprite = player->AddComponent<C_Sprite>();
//     // sprite->Load("assets/spritesheets/player/player_idle.png", true);
//     // sprite->Load("assets/spritesheets/player/player_walk.png", true);
//
//     auto animation = player->AddComponent<C_Animation>();
//
//     const int frameWidth = 48;
//     const int frameHeight = 48;
//
//     std::shared_ptr<Animation> idleAnimation = std::make_shared<Animation>();
//     const float idleAnimFrameSeconds = 0.2f;
//     idleAnimation->AddFrame(0, 0, frameWidth, frameHeight, idleAnimFrameSeconds, "assets/spritesheets/player/player_idle.png");
//     idleAnimation->AddFrame(48, 0, frameWidth, frameHeight, idleAnimFrameSeconds, "assets/spritesheets/player/player_idle.png");
//     idleAnimation->AddFrame(96, 0, frameWidth, frameHeight, idleAnimFrameSeconds, "assets/spritesheets/player/player_idle.png");
//     idleAnimation->AddFrame(144, 0, frameWidth, frameHeight, idleAnimFrameSeconds, "assets/spritesheets/player/player_idle.png");
//     idleAnimation->AddFrame(192, 0, frameWidth, frameHeight, idleAnimFrameSeconds, "assets/spritesheets/player/player_idle.png");
//     idleAnimation->AddFrame(240, 0, frameWidth, frameHeight, idleAnimFrameSeconds, "assets/spritesheets/player/player_idle.png");
//     idleAnimation->AddFrame(288, 0, frameWidth, frameHeight, idleAnimFrameSeconds, "assets/spritesheets/player/player_idle.png");
//     idleAnimation->AddFrame(336, 0, frameWidth, frameHeight, idleAnimFrameSeconds, "assets/spritesheets/player/player_idle.png");
//     idleAnimation->AddFrame(384, 0, frameWidth, frameHeight, idleAnimFrameSeconds, "assets/spritesheets/player/player_idle.png");
//     idleAnimation->AddFrame(432, 0, frameWidth, frameHeight, idleAnimFrameSeconds, "assets/spritesheets/player/player_idle.png");
//     animation->AddAnimation("idle", idleAnimation);
//
//     std::shared_ptr<Animation> walkAnimation = std::make_shared<Animation>();
//     const float walkAnimFrameSeconds = 0.2f;
//     walkAnimation->AddFrame(0, 0, frameWidth, frameHeight, idleAnimFrameSeconds, "assets/spritesheets/player/player_walk.png");
//     walkAnimation->AddFrame(48, 0, frameWidth, frameHeight, idleAnimFrameSeconds, "assets/spritesheets/player/player_walk.png");
//     walkAnimation->AddFrame(96, 0, frameWidth, frameHeight, idleAnimFrameSeconds, "assets/spritesheets/player/player_walk.png");
//     walkAnimation->AddFrame(144, 0, frameWidth, frameHeight, idleAnimFrameSeconds, "assets/spritesheets/player/player_walk.png");
//     walkAnimation->AddFrame(192, 0, frameWidth, frameHeight, idleAnimFrameSeconds, "assets/spritesheets/player/player_walk.png");
//     walkAnimation->AddFrame(240, 0, frameWidth, frameHeight, idleAnimFrameSeconds, "assets/spritesheets/player/player_walk.png");
//     walkAnimation->AddFrame(288, 0, frameWidth, frameHeight, idleAnimFrameSeconds, "assets/spritesheets/player/player_walk.png");
//     walkAnimation->AddFrame(336, 0, frameWidth, frameHeight, idleAnimFrameSeconds, "assets/spritesheets/player/player_walk.png");
//     animation->AddAnimation("walk", walkAnimation);
//     animation->PlayAnimation("walk");
//
//     player->GetDrawable()->SetSortOrder(1);
//     gameObjects.Add(player);
//
//     // ----------- ChildObject -----------
//     // std::shared_ptr<GameObject> childObject = std::make_shared<GameObject>("childObject");
//     // auto childSprite = childObject->AddComponent<C_Sprite>();
//     // childSprite->Load("assets/random/tabby.png", false);
//     // childObject->GetDrawable()->SetSortOrder(-1);
//     //
//     // player->AddChild(childObject);
//
//     // ----------- Title -----------
//     std::shared_ptr<GameObject> title = std::make_shared<GameObject>("title");
//     auto titleSprite = title->AddComponent<C_Sprite>();
//     titleSprite->Load("assets/random/tabby.png", false);
//
//     title->transform->SetPosition({ 500.0f, 400.f });
//     title->transform->SetScale({ 4.0f, 4.0f });
//
//     gameObjects.Add(title);
//
//     creationDone = true;
// }
//
// void TitleScreenScene::OnActivate()
// {
//
//     currentSeconds = 0.f;
//
//     activationDone = true;
// }
//
// void TitleScreenScene::OnDestroy() { }
//
// void TitleScreenScene::SetSwitchToScene(unsigned int id) { switchToState = id; }
//
// void TitleScreenScene::LateUpdate(float dt) { gameObjects.LateUpdate(dt); }
//
// void TitleScreenScene::Update(float dt)
// {
//
//     gameObjects.ProcessRemovals();
//     gameObjects.ProcessNewObjects();
//     gameObjects.Update(dt);
//
//     currentSeconds += dt;
//
//     if (currentSeconds >= showForSeconds) {
//
//         sceneStateMachine.SwitchTo(switchToState);
//     }
// }
//
// void TitleScreenScene::Draw()
// {
//     gameObjects.Draw();
// }
