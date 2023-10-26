#pragma once

#include "Scene/GameObject.h"
#include <Tabby.h>

class SceneHierarchyPanel {
public:
    SceneHierarchyPanel() = default;
    SceneHierarchyPanel(Tabby::Scene& scene);

    void SetContext(Tabby::Scene& scene);

    void OnImGuiRender();

    Tabby::GameObject GetSelectedEntity() const { return m_SelectionContext; }
    void SetSelectedEntity(Tabby::GameObject entity);

private:
    template <typename T>
    void DisplayAddComponentEntry(const std::string& entryName);

    void DrawEntityNode(Tabby::GameObject entity);
    void DrawComponents(Tabby::GameObject entity);

private:
    Tabby::Scene* m_Context;
    Tabby::GameObject m_SelectionContext;
};
