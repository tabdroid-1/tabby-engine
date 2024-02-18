#pragma once

#include "Tabby/Core/GamepadCodes.h"
#include "Tabby/Core/KeyCodes.h"
#include "Tabby/Core/MouseCodes.h"
#include <tbpch.h>

#include <glm/glm.hpp>

struct _SDL_Joystick;
typedef struct _SDL_Joystick SDL_Joystick;

namespace Tabby {

struct GamepadInfo {
    std::string name = "";
    void* SDL_Gamepad = nullptr;
    bool isGamepad = false;
    bool ready = false;
    float deadZone = 0.035f;
    // int axisCount;
    // int buttonCount;
};

class Input {
public:
    static void Init();

    static Input* Get() { return s_Instance; }

    static bool IsKeyPressed(KeyCode key);
    static bool IsMouseButtonPressed(MouseCode button);
    static glm::vec2 GetMousePosition();
    static float GetMouseX();
    static float GetMouseY();

    // Controller Stuff
    static const GamepadInfo& GetGamepadInfo(int index);
    static void RefreshGamepads();
    static int GetNumOfGamepads();
    static float GetGamepadAxis(int index, GamepadAxis axis);
    static bool IsGamepadButtonPressed(int index, GamepadButtons button);

private:
    Input();

    GamepadInfo gamepads[MAX_GAMEPADS];

private:
    static Input* s_Instance;
};
}

// bool IsGamepadAvailable(int gamepad)
// {
//     bool result = false;
//
//     if ((gamepad < MAX_GAMEPADS) && CORE.Input.Gamepad.ready[gamepad]) result = true;
//
//     return result;
// }
//
// // Get axis movement vector for a gamepad
// float GetGamepadAxisMovement(int gamepad, int axis)
// {
//     float value = 0;
//
//     if ((gamepad < MAX_GAMEPADS) && CORE.Input.Gamepad.ready[gamepad] && (axis < MAX_GAMEPAD_AXIS) &&
//         (fabsf(CORE.Input.Gamepad.axisState[gamepad][axis]) > 0.1f)) value = CORE.Input.Gamepad.axisState[gamepad][axis];      // 0.1f = GAMEPAD_AXIS_MINIMUM_DRIFT/DELTA
//
//     return value;
// }
// // Get the last gamepad button pressed
// int GetGamepadButtonPressed(void)
// {
//     return CORE.Input.Gamepad.lastButtonPressed;
// }
//// Get gamepad axis count
// int GetGamepadAxisCount(int gamepad)
// {
//     return CORE.Input.Gamepad.axisCount[gamepad];
// }
