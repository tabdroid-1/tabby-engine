#pragma once

#include "tbpch.h"
#include <iostream>
#include <raylib.h>

#include "Scene/SceneStateMachine.h"

int main(int argc, char** argv);

namespace Tabby {

struct ApplicationCommandLineArgs {
    int Count = 0;
    char** Args = nullptr;

    const char* operator[](int index) const
    {
        TB_ASSERT(index < Count, "Index is bigger than argument cound!");
        return Args[index];
    }
};

struct ApplicationSpecification {
    std::string Name = "Tabby Application";
    std::string WorkingDirectory;
    ApplicationCommandLineArgs CommandLineArgs;
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

Application* CreateApplication(ApplicationCommandLineArgs args);
}
