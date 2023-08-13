#include "Core/Game.h"
#include <math.h>

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

void GameLoop();

Game game;

int main()
{
    game.Init();

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(GameLoop, 0, 1);
#else
    while (game.IsRunning()) {
        GameLoop();
    }
#endif

    return 0;
}

void GameLoop()
{
    game.Update();
    game.LateUpdate();
}

// int main()
// {
//     Game game;
//
//     while (game.IsRunning()) {
//         game.Update();
//         game.LateUpdate();
//     }
//
//     return 0;
// }
