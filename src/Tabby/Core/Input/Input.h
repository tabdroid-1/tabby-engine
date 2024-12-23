#pragma once
#include <tbpch.h>
#include <Tabby/Core/Events/MouseEvent.h>
#include <Tabby/Core/Input/KeyCode.h>
#include <Tabby/Foundation/Types.h>

struct _SDL_Joystick;
typedef struct _SDL_Joystick SDL_Joystick;

namespace Tabby {

struct GamepadInfo {
    std::string name = "";
    void* SDL_Gamepad = nullptr;
    bool isGamepad = false;
    bool ready = false;
    float deadZone = 0.1f;
};

class Input {
public:
    static void Init();

    static bool GetKeyDown(KeyCode key);
    static bool GetKeyUp(KeyCode key);
    static bool GetKey(KeyCode key);
    static bool GetMouseButtonDown(MouseCode button);
    static bool GetMouseButtonUp(MouseCode button);
    static bool GetMouseButton(MouseCode button);
    // static bool IsKeyPressed(KeyCode key);
    // static bool IsMouseButtonPressed(MouseCode button);
    static Vector2 GetMousePosition();
    static Vector2 GetMouseScrollDelta();
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
    static void Update();

    GamepadInfo* m_Gamepads;
    Vector2 m_MouseScrollDelta;
    std::unordered_map<KeyCode, bool> currentKeyStates;
    std::unordered_map<KeyCode, bool> previousKeyStates;
    std::unordered_map<MouseCode, bool> currentMouseButtonStates;
    std::unordered_map<MouseCode, bool> previousMouseButtonStates;

private:
    inline static Input* s_Instance;

    friend class Application;
};

}
