#pragma once

#include "tbpch.h"
#include <iostream>
#include <raylib.h>

#include "Scene/SceneStateMachine.h"

int main(int argc, char** argv);

namespace Tabby {

struct ApplicationSpecification {
    std::string Name = "Tabby Application";
    std::string WorkingDirectory;
};

class Application {
public:
    Application(const ApplicationSpecification& specification);
    ~Application();

    void Run();

    bool IsRunning() const { return !WindowShouldClose() && isRunning; }

    static void CloseApplication()
    {
        s_Instance->isRunning = false;
    }
    static Application& Get() { return *s_Instance; }

public:
    RenderTexture2D frameBuffer;
    SceneStateMachine sceneManager;

private:
    bool isRunning = true;
    ApplicationSpecification m_Specification;

private:
    static Application* s_Instance;
    friend int ::main(int argc, char** argv);
};

Application* CreateApplication();
}
