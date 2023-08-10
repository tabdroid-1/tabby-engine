#pragma once

#include <LDtkLoader/Project.hpp>
#include <memory>
#include <raylib.h>
#include <string>

#include <ComponentSystem/Component.h>
#include <ComponentSystem/Components/C_Drawable.h>
#include <Scene/GameObject.h>

class C_LDtkTilemap : public Component, public C_Drawable {
public:
    C_LDtkTilemap(GameObject* owner);

    void Load(const std::string& filePath, const std::string& levelName);
    void Draw() override;
    void Delete() override;

public:
    void SetTint(Color color);

private:
    ldtk::Project ldtkProject {};
    const ldtk::World* ldtkWorld {};
    const ldtk::Level* ldtkLevel {};
    Texture2D tilesetTexture;
    Texture2D renderedLevelTexture;

    Color tint;
    bool isLoaded;
};
