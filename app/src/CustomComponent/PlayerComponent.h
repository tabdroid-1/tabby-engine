#pragma once

#include <Tabby.h>
#include <Tabby/World/Plugin.h>

struct PlayerComponent {
    bool asd;
};

struct PlayerPlugin : public Tabby::Plugin {
    void Build() override
    {
        auto ent = Tabby::World::CreateEntity("Test");
        ent.AddComponent<PlayerComponent>();
        TB_INFO("Plugin Built");
    }
};
