#include <Tabby.h>

#include <LevelEditorLayer.h>

class LevelEditorApplication : public Tabby::Application {
public:
    LevelEditorApplication(const Tabby::ApplicationSpecification& specification)
        : Tabby::Application(specification)
    {
        PushLayer(new Tabby::LevelEditorLayer());
    }

    ~LevelEditorApplication()
    {
    }
};

Tabby::Application* Tabby::CreateApplication(Tabby::ApplicationCommandLineArgs args)
{
    ApplicationSpecification spec;
    spec.Name = "LevelEditor";
    spec.WorkingDirectory = "assets";
    spec.CommandLineArgs = args;
    spec.maxFPS = 0.0f;

    return new LevelEditorApplication(spec);
}

#include <Tabby/Core/EntryPoint.h>
