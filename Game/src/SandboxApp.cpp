// #include <Core/EntryPoint.h>
#include <Game/Scenes/Scenes.h>
#include <Tabby.h>
#include <cstdio>

namespace Tabby {

class Sandbox : public Application {
public:
    Sandbox(const ApplicationSpecification& specification)
        : Application(specification)
    {

        // SetTargetFPS(520);
        ClearWindowState(FLAG_VSYNC_HINT);

        ToggleFullscreen();

        std::shared_ptr<TitleScreenScene> splashScreen = std::make_shared<TitleScreenScene>(sceneManager);
        // std::shared_ptr<DevTestScene> gameScene = std::make_shared<DevTestScene>(sceneManager);
        //
        unsigned int splashScreenID = sceneManager.Add(splashScreen);
        // unsigned int gameSceneID = sceneManager.Add(gameScene);
        //
        // splashScreen->SetSwitchToScene(gameSceneID);

        sceneManager.SwitchTo(splashScreenID);
    }

    ~Sandbox()
    {
    }
};

// Application* CreateApplication(ApplicationCommandLineArgs args)
Application* CreateApplication()
{

    ApplicationSpecification spec;
    spec.Name = "Tabby Engine - Library Test";
    // spec.CommandLineArgs = args;

    return new Sandbox(spec);
}

}

int main(int argc, char** argv)
{
    auto* app = Tabby::CreateApplication();

    app->Run();

    delete app;
    return 0;
}
