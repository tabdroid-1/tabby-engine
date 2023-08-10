#include "Core/Game.h"
#include <math.h>

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

int main()
{
    Game game;

    while (game.IsRunning()) {
        game.Update();
        game.LateUpdate();
    }

    return 0;
}
