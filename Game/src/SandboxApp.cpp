#include <Tabby.h>
#include <Tabby/Core/EntryPoint.h>

#include "Sandbox2D.h"
#include "ExampleLayer.h"

class Sandbox : public Tabby::Application
{
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
	spec.Name = "Game";
	spec.WorkingDirectory = "../Tabbyb";
	spec.CommandLineArgs = args;

	return new Sandbox(spec);
}
