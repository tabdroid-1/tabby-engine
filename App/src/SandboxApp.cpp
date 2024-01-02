#include <Tabby.h>
#include <Tabby/Core/EntryPoint.h>

// #include "ExampleLayer.h"
#include "Sandbox2D.h"

class Sandbox : public Tabby::Application {
public:
    Sandbox(const Tabby::ApplicationSpecification& specification)
        : Tabby::Application(specification)
    {
        // PushLayer(new ExampleLayer());
        PushLayer(new Sandbox2D());
    }

    ~Sandbox()
    {
    }
};

Tabby::Application* Tabby::CreateApplication(Tabby::ApplicationCommandLineArgs args)
{
    ApplicationSpecification spec;
    spec.Name = "Sandbox";
    spec.WorkingDirectory = "";
    spec.CommandLineArgs = args;

    return new Sandbox(spec);
}
