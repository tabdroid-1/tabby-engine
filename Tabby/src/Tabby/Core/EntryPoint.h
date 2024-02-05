#pragma once

#include "Application.h"
#include <Tabby/Core/Application.h>

extern Tabby::Application* Tabby::CreateApplication(ApplicationCommandLineArgs args);

#if !defined(TB_PLATFORM_WEB)
int main(int argc, char** argv)
{
    Tabby::Log::Init();

    auto app = Tabby::CreateApplication({ argc, argv });

    app->Run();

    delete app;
}

#else

#include <emscripten.h>

void run()
{
    Tabby::Application::Get().Run();
}

int main(int argc, char** argv)
{
    Tabby::Log::Init();

    auto app = Tabby::CreateApplication({ argc, argv });

    emscripten_set_main_loop((em_callback_func)run, 0, 1);

    delete app;
}
#endif
