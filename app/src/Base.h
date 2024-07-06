#pragma once

#include <Tabby.h>
#include <Panels/SceneHierarchyPanel/SceneHierarchyPanel.h>
#include <Panels/SceneHierarchyPanel/Properties.h>

class Base : public Tabby::Layer {
public:
    Base();
    virtual ~Base() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;

    void OnUpdate(Tabby::Timestep ts) override;
    virtual void OnImGuiRender() override;
    void OnEvent(Tabby::Event& e) override;

    void OnOverlayRender();

private:
    Tabby::SceneHierarchyPanel m_SceneHierarchyPanel;
    Tabby::PropertiesPanel m_PropertiesPanel;
    bool m_ShowPhysicsColliders = true;
    int m_GizmoType = -1;

    Tabby::Shared<Tabby::Framebuffer> m_Framebuffer;
    glm::vec2 m_ViewportSize = { 1280.0f, 720.0f };
    glm::vec2 m_ViewportBounds[2];
    bool m_ViewportFocused = false, m_ViewportHovered = false;
};
