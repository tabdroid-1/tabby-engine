#include "Game/Scripts/playerMove.h"
#include "fmt/core.h"
#include <ComponentSystem/Components.h>
#include <Game/Scenes/TitleScreenScene.h>
#include <memory>

TitleScreenScene::TitleScreenScene(SceneStateMachine& sceneStateMachine)
    : sceneStateMachine(sceneStateMachine)
    , switchToState(0)
    , currentSeconds(0.f)
    , showForSeconds(1.f)
{
}

void TitleScreenScene::OnCreate()
{

    // ----------- Player -----------
    std::shared_ptr<GameObject> player = std::make_shared<GameObject>("player", true);
    player->transform->SetPosition({ 500.0f, 400.f });
    player->transform->SetSize({ 48.0f, 48.f });
    player->transform->SetScale({ 4.0f, 4.f });

    auto sprite = player->AddComponent<C_Sprite>();
    // sprite->Load("assets/spritesheets/player/player_idle.png", true);
    // sprite->Load("assets/spritesheets/player/player_walk.png", true);

    auto animation = player->AddComponent<C_Animation>();

    const int frameWidth = 48;
    const int frameHeight = 48;

    std::shared_ptr<Animation> idleAnimation = std::make_shared<Animation>();
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
    animation->AddAnimation("idle", idleAnimation);

    std::shared_ptr<Animation> walkAnimation = std::make_shared<Animation>();
    const float walkAnimFrameSeconds = 0.2f;
    walkAnimation->AddFrame(0, 0, frameWidth, frameHeight, idleAnimFrameSeconds, "assets/spritesheets/player/player_walk.png");
    walkAnimation->AddFrame(48, 0, frameWidth, frameHeight, idleAnimFrameSeconds, "assets/spritesheets/player/player_walk.png");
    walkAnimation->AddFrame(96, 0, frameWidth, frameHeight, idleAnimFrameSeconds, "assets/spritesheets/player/player_walk.png");
    walkAnimation->AddFrame(144, 0, frameWidth, frameHeight, idleAnimFrameSeconds, "assets/spritesheets/player/player_walk.png");
    walkAnimation->AddFrame(192, 0, frameWidth, frameHeight, idleAnimFrameSeconds, "assets/spritesheets/player/player_walk.png");
    walkAnimation->AddFrame(240, 0, frameWidth, frameHeight, idleAnimFrameSeconds, "assets/spritesheets/player/player_walk.png");
    walkAnimation->AddFrame(288, 0, frameWidth, frameHeight, idleAnimFrameSeconds, "assets/spritesheets/player/player_walk.png");
    walkAnimation->AddFrame(336, 0, frameWidth, frameHeight, idleAnimFrameSeconds, "assets/spritesheets/player/player_walk.png");
    animation->AddAnimation("walk", walkAnimation);
    animation->PlayAnimation("walk");

    player->GetDrawable()->SetSortOrder(1);
    gameObjects.Add(player);

    // ----------- ChildObject -----------
    // std::shared_ptr<GameObject> childObject = std::make_shared<GameObject>("childObject");
    // auto childSprite = childObject->AddComponent<C_Sprite>();
    // childSprite->Load("assets/random/tabby.png", false);
    // childObject->GetDrawable()->SetSortOrder(-1);
    //
    // player->AddChild(childObject);

    // ----------- Title -----------
    std::shared_ptr<GameObject> title = std::make_shared<GameObject>("title");
    auto titleSprite = title->AddComponent<C_Sprite>();
    titleSprite->Load("assets/random/tabby.png", false);

    title->transform->SetPosition({ 500.0f, 400.f });
    title->transform->SetScale({ 4.0f, 4.0f });

    gameObjects.Add(title);
}

void TitleScreenScene::OnActivate() { currentSeconds = 0.f; }

void TitleScreenScene::OnDestroy() { }

void TitleScreenScene::SetSwitchToScene(unsigned int id) { switchToState = id; }

void TitleScreenScene::LateUpdate(float dt) { gameObjects.LateUpdate(dt); }

void TitleScreenScene::Update(float dt)
{

    gameObjects.ProcessRemovals();
    gameObjects.ProcessNewObjects();
    gameObjects.Update(dt);

    currentSeconds += dt;

    if (currentSeconds >= showForSeconds) {
        sceneStateMachine.SwitchTo(switchToState);
    }
}

void TitleScreenScene::Draw()
{
    gameObjects.Draw();
}
