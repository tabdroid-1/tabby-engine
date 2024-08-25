#include <Tabby/Core/Input/Input.h>
#include <Tabby/Core/Application.h>
#include <Tabby/Math/Math.h>

#include <SDL.h>

namespace Tabby {

void Input::Init()
{
    TB_PROFILE_SCOPE_NAME("Tabby::Input::Init");
    if (s_Instance == nullptr)
        s_Instance = new Input();
}

Input::Input()
{
    TB_PROFILE_SCOPE_NAME("Tabby::Input::Constructor");
    s_Instance = this;

    m_Gamepads = (GamepadInfo*)malloc(Application::GetSpecification().MaxGamepads * sizeof(GamepadInfo));

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
            m_Gamepads[i] = tempCont;
        } else
            TB_CORE_WARN("Unable to open game controller [ERROR: {0}]", SDL_GetError());
    }
}

void Input::Update()
{
    // Update key states
    const Uint8* keyState = SDL_GetKeyboardState(nullptr);
    for (auto& key : s_Instance->currentKeyStates) {
        s_Instance->previousKeyStates[key.first] = key.second;
        key.second = keyState[static_cast<SDL_Scancode>(key.first)] == 1;
    }

    // Update mouse button states
    Uint32 mouseState = SDL_GetMouseState(nullptr, nullptr);
    for (auto& button : s_Instance->currentMouseButtonStates) {
        s_Instance->previousMouseButtonStates[button.first] = button.second;
        button.second = (mouseState & SDL_BUTTON(static_cast<int>(button.first))) != 0;
    }

    s_Instance->m_MouseScrollDelta = { 0, 0 };
}

}
