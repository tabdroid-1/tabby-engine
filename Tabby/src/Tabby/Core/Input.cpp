#include "Input.h"
#include "SDL.h"

namespace Tabby {

Input* Input::s_Instance = nullptr;

void Input::Init()
{
    if (s_Instance == nullptr)
        s_Instance = new Input();
}

Input::Input()
{
    for (int i = 0; (i < SDL_NumJoysticks()) && (i < MAX_GAMEPADS); i++) {
        std::unique_ptr<Controller> tempCont = std::make_unique<Controller>();

        // Check if joystick is a gamepad (ex. xbox360 and ps3 controllers)
        if (SDL_IsGameController(i)) {
            tempCont->SDL_Gamepad = SDL_GameControllerOpen(i);
            tempCont->isGamepad = true;
        } else {
            tempCont->SDL_Gamepad = SDL_JoystickOpen(i);
            tempCont->isGamepad = false;
        }

        if (tempCont->SDL_Gamepad) {
            tempCont->ready = true;
            if (tempCont->isGamepad) {
            } else {
                tempCont->axisCount = SDL_JoystickNumAxes((SDL_Joystick*)tempCont->SDL_Gamepad);
                tempCont->buttonCount = SDL_JoystickNumButtons((SDL_Joystick*)tempCont->SDL_Gamepad);
                tempCont->name = SDL_JoystickName((SDL_Joystick*)tempCont->SDL_Gamepad);
            }
            tempCont->name += '\0';
            gamepad[i] = tempCont.get();
        } else
            TB_CORE_WARN("Unable to open game controller [ERROR: {0}]", SDL_GetError());
    }
}

void Input::RefreshGamepads()
{
    // for (int i = 0; (i < SDL_NumJoysticks()) && (i < MAX_GAMEPADS); i++) {
    //     s_Instance->gamepad[i]->SDL_Gamepad = SDL_JoystickOpen(i);
    //     if (s_Instance->gamepad[i]->SDL_Gamepad) {
    //         s_Instance->gamepad[i]->ready = true;
    //         s_Instance->gamepad[i]->axisCount = SDL_JoystickNumAxes(s_Instance->gamepad[i]->SDL_Gamepad);
    //         s_Instance->gamepad[i]->buttonCount = SDL_JoystickNumButtons(s_Instance->gamepad[i]->SDL_Gamepad);
    //         s_Instance->gamepad[i]->axisState[Gamepad::GAMEPAD_AXIS_LEFT_TRIGGER] = -1.0f;
    //         s_Instance->gamepad[i]->axisState[Gamepad::GAMEPAD_AXIS_RIGHT_TRIGGER] = -1.0f;
    //         s_Instance->gamepad[i]->name = SDL_JoystickName(s_Instance->gamepad[i]->SDL_Gamepad);
    //         s_Instance->gamepad[i]->name += '\0';
    //     } else
    //         TB_CORE_WARN("Unable to open game controller [ERROR: {0}]", SDL_GetError());
    // }
}
}
