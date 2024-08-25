#include <tbpch.h>
#include <Tabby/Core/Input/Input.h>
#include <Tabby/Core/Application.h>
#include <Tabby/Math/Math.h>

#include <SDL.h>

namespace Tabby {

void Input::RefreshGamepads()
{
}

const GamepadInfo& Input::GetGamepadInfo(int index)
{
    TB_PROFILE_SCOPE_NAME("Tabby::Input::GetGamepadInfo");

    return s_Instance->m_Gamepads[index];
}

bool Input::GetKeyDown(KeyCode key)
{
    return false;
}

bool Input::GetKeyUp(KeyCode key)
{
    return false;
}

bool Input::GetKey(KeyCode key)
{
    return false;
}

bool Input::GetMouseButtonDown(MouseCode button)
{
    return false;
}

bool Input::GetMouseButtonUp(MouseCode button)
{
    return false;
}

bool Input::GetMouseButton(MouseCode button)
{
    return false;
}

Vector2 Input::GetMouseScrollDelta()
{
    return { 0.0f, 0.0f };
}

Vector2 Input::GetMousePosition()
{
    return { 0.0f, 0.0f };
}

float Input::GetMouseX()
{
    return 0.0f;
}

float Input::GetMouseY()
{
    return 0.0f;
}

// Controller
int Input::GetNumOfGamepads()
{
    return 0;
}

float Input::GetGamepadAxis(int index, GamepadAxis axis)
{
    return 0;
}

bool Input::IsGamepadButtonPressed(int index, GamepadButtons button)
{
    return false;
}

}
