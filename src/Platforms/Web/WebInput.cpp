#include <tbpch.h>
#include <Tabby/Core/Input/Input.h>
#include <Tabby/Core/Application.h>
#include <Tabby/Math/Math.h>

#include <SDL.h>

namespace Tabby {

void Input::RefreshGamepads()
{
    TB_PROFILE_SCOPE_NAME("Tabby::Input::RefreshGamepads");

    for (int i = 0; (i < SDL_NumJoysticks()) && (i < Application::GetSpecification().MaxGamepads); i++) {
        GamepadInfo tempCont;

        // Check if joystick is a gamepad (ex. xbox360 and ps3 controllers)
        if (SDL_IsGameController(i)) {
            tempCont.SDL_Gamepad = SDL_GameControllerOpen(i);
            tempCont.isGamepad = true;
        } else {
            tempCont.SDL_Gamepad = SDL_JoystickOpen(i);
            tempCont.isGamepad = false;
        }

        if (tempCont.SDL_Gamepad) {
            tempCont.ready = true;
            if (tempCont.isGamepad) {
                tempCont.name = SDL_GameControllerName((SDL_GameController*)tempCont.SDL_Gamepad);
            } else {
                tempCont.name = SDL_JoystickName((SDL_Joystick*)tempCont.SDL_Gamepad);
            }
            tempCont.name += '\0';
            s_Instance->m_Gamepads[i] = tempCont;
        } else
            TB_CORE_WARN("Unable to open game controller [ERROR: {0}]", SDL_GetError());
    }
}

const GamepadInfo& Input::GetGamepadInfo(int index)
{
    TB_PROFILE_SCOPE_NAME("Tabby::Input::GetGamepadInfo");

    return s_Instance->m_Gamepads[index];
}

bool Input::GetKeyDown(KeyCode key)
{
    return s_Instance->currentKeyStates[key] && !s_Instance->previousKeyStates[key];
}

bool Input::GetKeyUp(KeyCode key)
{
    return !s_Instance->currentKeyStates[key] && s_Instance->previousKeyStates[key];
}

bool Input::GetKey(KeyCode key)
{
    return s_Instance->currentKeyStates[key];
}

bool Input::GetMouseButtonDown(MouseCode button)
{
    return s_Instance->currentMouseButtonStates[button] && !s_Instance->previousMouseButtonStates[button];
}

bool Input::GetMouseButtonUp(MouseCode button)
{
    return !s_Instance->currentMouseButtonStates[button] && s_Instance->previousMouseButtonStates[button];
}

bool Input::GetMouseButton(MouseCode button)
{
    return s_Instance->currentMouseButtonStates[button];
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
