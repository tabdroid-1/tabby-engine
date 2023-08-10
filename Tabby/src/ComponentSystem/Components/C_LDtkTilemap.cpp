#include "C_LDtkTilemap.h"
#include "raylib.h"
#include <cstdlib>

C_LDtkTilemap::C_LDtkTilemap(GameObject* owner)
    : Component(owner)
    , isLoaded(false)
    , tint(WHITE)
{
}

/* Load LDtk tilemap*/
void C_LDtkTilemap::Load(const std::string& filePath,
    const std::string& level)
{
    if (!isLoaded) {
        ldtkProject.loadFromFile(filePath);
        ldtkWorld = &ldtkProject.getWorld();
        ldtkLevel = &ldtkWorld->getLevel(level);

        isLoaded = true;
    }
    auto renderTexture = LoadRenderTexture(ldtkLevel->size.x, ldtkLevel->size.y);

    BeginTextureMode(renderTexture);

    // if (ldtkLevel->hasBgImage()) {
    //     std::string backgroundPath = ldtkLevel->getBgImage().path.c_str();
    //     std::string path = "assets/tilemaps/" + backgroundPath;
    //     auto backgroundTexture = LoadTexture(path.c_str());
    //     SetTextureFilter(backgroundTexture, TEXTURE_FILTER_TRILINEAR);
    //
    //     DrawTextureV(backgroundTexture, { float(backgroundTexture.width), float(backgroundTexture.height) }, WHITE);
    // }

    // draw all tileset layers
    for (auto&& layer : ldtkLevel->allLayers()) {
        if (layer.hasTileset()) {
            std::string textureName = layer.getTileset().path;
            std::string path = "assets/tilemaps/" + textureName;
            tilesetTexture = LoadTexture(path.c_str());
            // if it is a tile layer then draw every tile to the frame buffer
            for (auto&& tile : layer.allTiles()) {
                auto source_pos = tile.getTextureRect();
                auto tile_size = float(layer.getTileset().tile_size);

                Vector2 target_pos = { (float)tile.getPosition().x,
                    (float)tile.getPosition().y };

                Rectangle source_rect = {
                    .x = float(source_pos.x),
                    .y = float(source_pos.y),
                    .width = tile.flipX ? -tile_size : tile_size,
                    .height = tile.flipY ? -tile_size : tile_size,
                };

                Rectangle dest_rect = { .x = float(target_pos.x),
                    .y = float(target_pos.y),
                    .width = tile_size,
                    .height = tile_size };

                DrawTextureRec(tilesetTexture, source_rect, target_pos, WHITE);
            }
        }
    }
    EndTextureMode();
    renderedLevelTexture = renderTexture.texture;
    isLoaded = true;
}

void C_LDtkTilemap::Draw()
{
    Rectangle source_rect = { 0, 0, (float)renderedLevelTexture.width,
        (float)-renderedLevelTexture.height };
    Rectangle dest_rect = {
        owner->transform->GetPosition().x, owner->transform->GetPosition().y,
        (float)(renderedLevelTexture.width * owner->transform->GetScale().x),
        (float)(renderedLevelTexture.height * owner->transform->GetScale().y)
    };

    DrawTexturePro(renderedLevelTexture, source_rect, dest_rect, { 0, 0 }, 0, tint);
}

void C_LDtkTilemap::Delete()
{
    // delete ldtkProject;
    delete ldtkWorld;
    delete ldtkLevel;
    UnloadTexture(tilesetTexture);
    UnloadTexture(renderedLevelTexture);
}

void C_LDtkTilemap::SetTint(Color tint) { this->tint = tint; }
