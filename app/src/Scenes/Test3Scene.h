#pragma once

#include "Tabby.h"

class Test3Scene : public Tabby::Scene {
public:
    Test3Scene();

    void OnCreate() override;
    void OnActivate() override;

    void DrawImGui() override;
};
