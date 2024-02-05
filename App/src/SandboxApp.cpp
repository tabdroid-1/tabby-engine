#include <Base.h>
#include <Tabby.h>

// #include "ExampleLayer.h"
#include "Sandbox2D.h"

class Sandbox : public Tabby::Application {
public:
    Sandbox(const Tabby::ApplicationSpecification& specification)
        : Tabby::Application(specification)
    {
        // PushLayer(new Sandbox2D());
        PushLayer(new Base());
        // PushLayer(new GameOfLifeBase());
    }

    ~Sandbox()
    {
    }
};

Tabby::Application* Tabby::CreateApplication(Tabby::ApplicationCommandLineArgs args)
{
    ApplicationSpecification spec;
    spec.Name = "A winfow";
    spec.WorkingDirectory = "";
    spec.CommandLineArgs = args;

    return new Sandbox(spec);
}

#ifdef TB_PLATFORM_WEB

#include <emscripten.h>

void run()
{
    Tabby::Application::Get().Run();
}

int main(int argc, char** argv)
{
    Tabby::Log::Init();

    auto app = Tabby::CreateApplication({ argc, argv });

    emscripten_set_main_loop(&run, 0, 1);

    delete app;
}
#else

#include <Tabby/Core/EntryPoint.h>

#endif
