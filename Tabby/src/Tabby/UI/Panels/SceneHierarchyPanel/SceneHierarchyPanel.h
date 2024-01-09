#pragma once

#include "Tabby/Core/Base.h"
#include "Tabby/Scene/Entity.h"
#include "Tabby/Scene/Scene.h"

namespace Tabby {

class SceneHierarchyPanel {
public:
    SceneHierarchyPanel() = default;
    SceneHierarchyPanel(const Ref<Scene>& scene);

    void SetContext(const Ref<Scene>& scene);

    void OnImGuiRender();

    Entity GetSelectedEntity() const { return m_SelectionContext; }
    void SetSelectedEntity(Entity entity);

private:
    template <typename T>
    void DisplayAddComponentEntry(const std::string& entryName);

    void DrawEntityNode(Entity entity);
    void DrawComponents(Entity entity);

private:
    Ref<Scene> m_Context;
    Entity m_SelectionContext;
};

}
