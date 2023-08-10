#include "Core/Game.h"

int main()
{
    Game game;

    while (game.IsRunning()) {
        game.Update();
        game.LateUpdate();
    }

    return 0;
}
