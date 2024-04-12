#pragma once

#include "Tabby/Input/GamepadCodes.h"
#include "Tabby/Input/KeyCodes.h"
#include "Tabby/Input/MouseCodes.h"
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
