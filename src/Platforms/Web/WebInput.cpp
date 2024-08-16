#ifdef TB_PLATFORM_WEB

#include "tbpch.h"

#include <Tabby/Core/Input/Input.h>
#include <Tabby/Math/Math.h>

#include <SDL.h>

namespace Tabby {

bool Input::IsKeyPressed(const KeyCode key)
{
    TB_PROFILE_SCOPE_NAME("Tabby::Input::IsKeyPressed");

    const Uint8* keyState = SDL_GetKeyboardState(nullptr);
    SDL_Scancode sdlKey = static_cast<SDL_Scancode>(key);
    return keyState[sdlKey] == 1;
}

bool Input::IsMouseButtonPressed(const MouseCode button)
{
    TB_PROFILE_SCOPE_NAME("Tabby::Input::IsMouseButtonPressed");

    Uint32 buttons = SDL_GetMouseState(NULL, NULL);
    return (buttons & SDL_BUTTON(static_cast<int>(button))) != 0;
}

Vector2 Input::GetMouseScrollDelta()
{
    TB_PROFILE_SCOPE_NAME("Tabby::Input::GetMouseScrollDelta");

    return s_Instance->m_MouseScrollDelta;
}

Vector2 Input::GetMousePosition()
{
    TB_PROFILE_SCOPE_NAME("Tabby::Input::GetMousePosition");

    int x, y;
    SDL_GetMouseState(&x, &y);
    return { static_cast<float>(x), static_cast<float>(y) };
}

float Input::GetMouseX()
{
    TB_PROFILE_SCOPE_NAME("Tabby::Input::GetMouseX");

    return GetMousePosition().x;
}

float Input::GetMouseY()
{
    TB_PROFILE_SCOPE_NAME("Tabby::Input::GetMouseY");

    return GetMousePosition().y;
}

// Controller
int Input::GetNumOfGamepads()
{
    TB_PROFILE_SCOPE_NAME("Tabby::Input::GetNumOfGamepads");

    return SDL_NumJoysticks();
}

float Input::GetGamepadAxis(int index, GamepadAxis axis)
{
    TB_PROFILE_SCOPE_NAME("Tabby::Input::GetGamepadAxis");

    float result = 0.0f;

    if (!s_Instance->m_Gamepads[index].SDL_Gamepad)
        return result;

    if (s_Instance->m_Gamepads[index].isGamepad) {
        result = SDL_GameControllerGetAxis((SDL_GameController*)s_Instance->m_Gamepads[index].SDL_Gamepad, static_cast<SDL_GameControllerAxis>(axis));
    } else {
        result = SDL_JoystickGetAxis((SDL_Joystick*)s_Instance->m_Gamepads[index].SDL_Gamepad, axis);
    }

    // Dividing to clamp it to be between -1 and 1
    result /= 32767.0f;

    if (Math::Abs(result) >= s_Instance->m_Gamepads[index].deadZone)
        return result;
    else
        return 0;
}

bool Input::IsGamepadButtonPressed(int index, GamepadButtons button)
{
    TB_PROFILE_SCOPE_NAME("Tabby::Input::IsGamepadButtonPressed");

    if (!s_Instance->m_Gamepads[index].SDL_Gamepad)
        return false;

    if (button == Gamepad::Buttons::GAMEPAD_BUTTON_LEFT_TRIGGER)
        return GetGamepadAxis(index, Gamepad::Axis::GAMEPAD_AXIS_LEFT_TRIGGER) > 0 ? true : false;
    else if (button == Gamepad::Buttons::GAMEPAD_BUTTON_RIGHT_TRIGGER)
        return GetGamepadAxis(index, Gamepad::Axis::GAMEPAD_AXIS_RIGHT_TRIGGER) > 0 ? true : false;

    if (s_Instance->m_Gamepads[index].isGamepad)
        return SDL_GameControllerGetButton((SDL_GameController*)s_Instance->m_Gamepads[index].SDL_Gamepad, static_cast<SDL_GameControllerButton>(button));
    else
        return SDL_JoystickGetButton((SDL_Joystick*)s_Instance->m_Gamepads[index].SDL_Gamepad, button);
}

}

#endif
