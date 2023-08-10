#pragma once

#include <memory>
#include <raylib-tmx.h>
#include <raylib.h>
#include <string>

#include <ComponentSystem/Component.h>
#include <ComponentSystem/Components/C_Transform.h>
#include <Scene/GameObject.h>

struct TiledTilemap {
    tmx_map* map;
    Color tint;
};

class C_TiledTilemap : public Component, public C_Drawable {
public:
    C_TiledTilemap(GameObject* owner);

    void Load(const std::string& tilemapPath);
    void Draw() override;
    void Delete() override;

public:
    void SetTint(Color color);

private:
    TiledTilemap tiledTilemap;
    bool isLoaded;
};
