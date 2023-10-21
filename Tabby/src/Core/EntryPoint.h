#pragma once

#include <Core/Application.h>

extern Tabby::Application* Tabby::CreateApplication(ApplicationCommandLineArgs args);

int main(int argc, char** argv)
{
    auto app = Tabby::CreateApplication({ argc, argv });

    app->Run();

    delete app;
}
