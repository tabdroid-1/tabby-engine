#include <Core/EntryPoint.h>
#include <Game/Scenes/Scenes.h>
#include <Tabby.h>
#include <cstdio>

namespace Tabby {

class Sandbox : public Application {
public:
    Sandbox(const ApplicationSpecification& specification)
        : Application(specification)
    {

        SetTargetFPS(1200);
        ClearWindowState(FLAG_VSYNC_HINT);

        ToggleFullscreen();

        std::shared_ptr<TitleScreenScene> splashScreen = std::make_shared<TitleScreenScene>();
        // std::shared_ptr<DevTestScene> gameScene = std::make_shared<DevTestScene>(sceneManager);
        //
        SceneStateMachine::Add("TitleScreen", splashScreen);
        // unsigned int splashScreenID = sceneManager.Add(splashScreen);
        // unsigned int gameSceneID = sceneManager.Add(gameScene);

        //
        // splashScreen->SetSwitchToScene(gameSceneID);

        SceneStateMachine::SwitchTo("TitleScreen");
    }

    ~Sandbox()
    {
    }
};

Application* CreateApplication(ApplicationCommandLineArgs args)
{

    ApplicationSpecification spec;
    spec.Name = "Tabby Engine - Test";
    spec.CommandLineArgs = args;

    return new Sandbox(spec);
}

}
