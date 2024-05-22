#pragma once

#include <Tabby/World/Entity.h>

namespace Tabby {

class SceneHierarchyPanel {
public:
    SceneHierarchyPanel() = default;
    ~SceneHierarchyPanel();

    void OnImGuiRender();
    void SetSelectedNode(Entity node, bool is_selected)
    {
        m_SelectedNode = node;
        m_IsSelected = is_selected;
    }

    Entity GetSelectedNode() const { return m_SelectedNode; }
    bool IsNodeSelected() const { return m_IsSelected; }

private:
    void RenderHierarchyNode(Entity entity);

private:
    Entity m_SelectedNode;
    bool m_IsSelected = false;

    bool m_IsOpen = true;
};

}
