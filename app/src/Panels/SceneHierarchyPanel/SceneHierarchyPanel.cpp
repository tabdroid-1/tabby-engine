#include <Tabby.h>

#include "SceneHierarchyPanel.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <misc/cpp/imgui_stdlib.h>

namespace Tabby {

SceneHierarchyPanel::~SceneHierarchyPanel()
{
}

void SceneHierarchyPanel::OnImGuiRender()
{
    if (m_IsOpen) {
        ImGui::Begin("Scene Hierarchy", &m_IsOpen);
        ImGui::Text("Right-click to create object");

        if (ImGui::BeginPopupContextWindow("hierarchy_create_entity_popup", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)) {
            if (ImGui::MenuItem("Create object")) {
                m_SelectedNode = World::CreateEntity();
                m_IsSelected = true;
            }
            ImGui::EndPopup();
        }
        ImGui::Separator();

        auto entities = World::GetRegistry().group<IDComponent, TagComponent>();
        for (auto& entity_id : entities) {
            Entity entity(entity_id);
            if (!entity.GetComponent<HierarchyNodeComponent>().Parent.first)
                RenderHierarchyNode(entity);
        };
        ImGui::End();
    }
}

void SceneHierarchyPanel::RenderHierarchyNode(Entity entity)
{
    TagComponent& tag_component = entity.GetComponent<TagComponent>();
    IDComponent& uc = entity.GetComponent<IDComponent>();
    HierarchyNodeComponent& hierarchy_node_component = entity.GetComponent<HierarchyNodeComponent>();

    ImGuiTreeNodeFlags flags = ((m_SelectedNode == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnDoubleClick
        | ImGuiTreeNodeFlags_OpenOnArrow;
    bool node_opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)(entt::entity)entity, flags, tag_component.Tag.c_str());

    if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
        ImGui::OpenPopup(fmt::format("hierarchy_node_popup{}", uc.ID).c_str());
    }

    if (ImGui::BeginPopup(fmt::format("hierarchy_node_popup{}", uc.ID).c_str())) {
        if (ImGui::MenuItem("Delete")) {
            World::DestroyEntity(entity);
            m_IsSelected = false;
            m_SelectedNode = (entt::entity)0;
        }
        if (ImGui::MenuItem("Delete with children")) {
            World::DestroyEntityWithChildren(entity);
            m_IsSelected = false;
            m_SelectedNode = (entt::entity)0;
        }
        if (ImGui::MenuItem("Create child")) {
            Entity e = World::CreateEntity();
            hierarchy_node_component.Children.push_back(std::pair(e.GetComponent<IDComponent>().ID, e));
            e.GetComponent<HierarchyNodeComponent>().Parent = std::pair(uc.ID, entity);
            m_IsSelected = true;
            m_SelectedNode = e;
        }
        ImGui::EndPopup();
    }

    ImGuiDragDropFlags drag_and_drop_flags = ImGuiDragDropFlags_None;
    drag_and_drop_flags |= ImGuiDragDropFlags_AcceptNoDrawDefaultRect;

    if (ImGui::BeginDragDropSource(drag_and_drop_flags)) {
        if (!(drag_and_drop_flags & ImGuiDragDropFlags_SourceNoPreviewTooltip))
            ImGui::Text(tag_component.Tag.c_str());
        ImGui::SetDragDropPayload("hierarchy_move_node_payload", &uc.ID, sizeof(uc.ID));
        ImGui::EndDragDropSource();
    }

    if (ImGui::BeginDragDropTarget()) {
        ImGuiDragDropFlags target_flags = 0;
        target_flags |= ImGuiDragDropFlags_AcceptNoDrawDefaultRect; // Don't display the yellow rectangle
        const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Scene_hierarchy_move_node_payload", target_flags);
        if (payload) {
            UUID id(*(uint64_t*)payload->Data);
            hierarchy_node_component.Children.push_back(std::pair(id, World::GetEntityByUUID(id)));

            Entity child = World::GetEntityByUUID(id);
            child.GetComponent<HierarchyNodeComponent>().Parent = std::pair(uc.ID, entity);
        }
        ImGui::EndDragDropTarget();
    }

    if (ImGui::IsItemClicked()) {
        if (Input::IsMouseButtonPressed(Mouse::ButtonLeft)) {
            m_SelectedNode = entity;
            m_IsSelected = true;
        }
    }
    if (node_opened) {
        for (auto& child : hierarchy_node_component.Children) {
            RenderHierarchyNode(child.second);
        }
        ImGui::TreePop();
    }
}

}
