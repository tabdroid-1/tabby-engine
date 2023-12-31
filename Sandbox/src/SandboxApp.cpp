#include <Tabby.h>
#include <Tabby/Core/EntryPoint.h>

#include "ExampleLayer.h"
#include "Sandbox2D.h"

class Sandbox : public Tabby::Application {
public:
    Sandbox()
    {
        // PushLayer(new ExampleLayer());
        PushLayer(new Sandbox2D());
    }

    ~Sandbox()
    {
    }
};

Tabby::Application* Tabby::CreateApplication()
{
    return new Sandbox();
}
