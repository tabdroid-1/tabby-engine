#pragma once

#include <Tabby/Core/Application.h>

extern Tabby::Application* Tabby::CreateApplication(ApplicationCommandLineArgs args);

int main(int argc, char** argv)
{
    Tabby::Log::Init();

    auto app = Tabby::CreateApplication({ argc, argv });

    app->Run();

    delete app;
}
