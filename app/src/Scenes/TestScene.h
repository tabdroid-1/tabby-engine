#pragma once

#include "Tabby.h"

class TestScene : public Tabby::Scene {
public:
    TestScene();

    void OnCreate() override;
    void OnActivate() override;

    void DrawImGui() override;
};
