#ifdef TB_PLATFORM_WINDOWS

#include "Tabby/Core/Input.h"
#include "tbpch.h"

#include "Tabby/Core/Application.h"
#include <SDL.h>

namespace Tabby {

bool Input::IsKeyPressed(const KeyCode key)
{
    const Uint8* keyState = SDL_GetKeyboardState(nullptr);
    SDL_Scancode sdlKey = static_cast<SDL_Scancode>(key);
    return keyState[sdlKey] == 1;
}

bool Input::IsMouseButtonPressed(const MouseCode button)
{
    Uint32 buttons = SDL_GetMouseState(NULL, NULL);
    return (buttons & SDL_BUTTON(static_cast<int>(button))) != 0;
}

glm::vec2 Input::GetMousePosition()
{
    int x, y;
    SDL_GetMouseState(&x, &y);
    return { static_cast<float>(x), static_cast<float>(y) };
}

float Input::GetMouseX()
{
    return GetMousePosition().x;
}

float Input::GetMouseY()
{
    return GetMousePosition().y;
}

}

#endif
