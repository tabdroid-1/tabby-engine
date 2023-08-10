#include "C_TiledTilemap.h"

C_TiledTilemap::C_TiledTilemap(GameObject* owner)
    : Component(owner)
    , isLoaded(false)
{

    tiledTilemap.tint = WHITE;
}

/* Load Tiled tilemap*/
void C_TiledTilemap::Load(const std::string& filePath)
{

    if (!isLoaded) {
        tiledTilemap.map = LoadTMX(filePath.c_str());

        isLoaded = true;
    }
}

void C_TiledTilemap::Draw()
{
    DrawTMX(tiledTilemap.map, owner->transform->GetPosition().x,
        owner->transform->GetPosition().y, tiledTilemap.tint);
}

void C_TiledTilemap::Delete() { delete tiledTilemap.map; }

void C_TiledTilemap::SetTint(Color tint) { tiledTilemap.tint = tint; }
