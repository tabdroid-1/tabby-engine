// #include <Core/EntryPoint.h>
#include <Game/Scenes/Scenes.h>
#include <Tabby.h>

#include "Sandbox2D.h"

namespace Tabby {

class Sandbox : public Application {
public:
    Sandbox(const ApplicationSpecification& specification)
        : Application(specification)
    {
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

Application* CreateApplication(ApplicationCommandLineArgs args)
{

    ApplicationSpecification spec;
    spec.Name = "Tabby Engine - Library Test";
    spec.CommandLineArgs = args;

    return new Sandbox(spec);
}

}

int main(int argc, char** argv)
{
    auto* app = Tabby::CreateApplication({ argc, argv });

    app->Run();

    delete app;
}
