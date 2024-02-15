#pragma once

#include "Application.h"
#include <Tabby/Core/Application.h>

extern Tabby::Application* Tabby::CreateApplication(ApplicationCommandLineArgs args);

#if defined(TB_PLATFORM_ANDROID)

#define SDL_MAIN_HANDLED
#include "../../../vendor/SDL2/include/SDL_main.h"
<<<<<<< HEAD
// int main(int argc, char** argv)
// {
//     // Tabby::Log::Init();
//
//     auto app = Tabby::CreateApplication({ argc, argv });
//
//     app->Run();
//
//     delete app;
// }

int SDL_main(int argc, char* argv[])
{
    // Tabby::Log::Init();

    auto app = Tabby::CreateApplication({ argc, argv });

    app->Run();

    delete app;

    return 0;
}

#elif defined(TB_PLATFORM_LINUX)
=======
>>>>>>> 0be7a4d412b3627228f337d78beff91934d6db7f
int main(int argc, char** argv)
{
    Tabby::Log::Init();

    auto app = Tabby::CreateApplication({ argc, argv });

    app->Run();

    delete app;
}

<<<<<<< HEAD
=======
int SDL_main(int argc, char* argv[])
{
    return main(argc, argv);
}

#elif defined(TB_PLATFORM_LINUX)
int main(int argc, char** argv)
{
    Tabby::Log::Init();

    auto app = Tabby::CreateApplication({ argc, argv });

    app->Run();

    delete app;
}

#elif defined(TB_PLATFORM_WINDOWS)
int main(int argc, char** argv)
{
    Tabby::Log::Init();

    auto app = Tabby::CreateApplication({ argc, argv });

    app->Run();

    delete app;
}

>>>>>>> 0be7a4d412b3627228f337d78beff91934d6db7f
#elif defined(TB_PLATFORM_MACOS)
int main(int argc, char** argv)
{
    Tabby::Log::Init();

    auto app = Tabby::CreateApplication({ argc, argv });

    app->Run();

    delete app;
}

#elif defined(TB_PLATFORM_WEB)

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
