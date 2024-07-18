#include <Tabby.h>
#include <Panels/SceneHierarchyPanel/Properties.h>
#include <Tabby/Renderer/Camera.h>

#include <imgui.h>
#include <imgui_internal.h>
#include <misc/cpp/imgui_stdlib.h>

#include <glm/glm.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Tabby {

void PropertiesPanel::OnImGuiRender()
{
    if (m_IsOpen) {
        ImGui::Begin("Properties", &m_IsOpen);
        if (m_Selected) {
            std::string& tag = m_Entity.GetComponent<TagComponent>().Tag;

            if (ImGui::BeginTable("properties_entity_name", 3, ImGuiTableFlags_SizingStretchProp)) {
                ImGui::TableNextColumn();
                ImGui::Text("Entity name: ");

                ImGui::TableNextColumn();
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                ImGui::InputText("##", tag.data(), tag.capacity());
                tag.resize(strlen(tag.c_str()));

                ImGui::TableNextColumn();
                if (ImGui::Button(" + ", { 0.0f, 0.0f })) {
                    ImGui::OpenPopup("Components popup");
                }

                if (ImGui::BeginPopup("Components popup")) {
                    ImGui::BeginDisabled(m_Entity.HasComponent<SpriteRendererComponent>());
                    if (ImGui::MenuItem("Sprite component")) {
                        SpriteRendererComponent& sprite_component = m_Entity.AddComponent<SpriteRendererComponent>();
                        // sprite_component.Texture = m_Context->GetRenderer()->GetDummyWhiteTexture();
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::EndDisabled();

                    ImGui::BeginDisabled(m_Entity.HasComponent<CameraComponent>());
                    if (ImGui::MenuItem("Camera component")) {
                        CameraComponent& camera_component = m_Entity.AddComponent<CameraComponent>();
                        // Shared<Camera> camera = std::make_shared<Camera3D>();
                        // camera->SetProjection(glm::radians(80.0f), 16.0 / 9.0f, 0.1f, 100.0f);
                        // camera_component.camera = camera;
                        camera_component.Primary = false;
                    }
                    ImGui::EndDisabled();

                    ImGui::BeginDisabled(m_Entity.HasComponent<Rigidbody2DComponent>());
                    if (ImGui::MenuItem("Rigid body 2D component"))
                        m_Entity.AddComponent<Rigidbody2DComponent>();
                    ImGui::EndDisabled();

                    ImGui::BeginDisabled(m_Entity.HasComponent<BoxCollider2DComponent>());
                    if (ImGui::MenuItem("Box collider component"))
                        m_Entity.AddComponent<BoxCollider2DComponent>();
                    ImGui::EndDisabled();

                    ImGui::BeginDisabled(m_Entity.HasComponent<CircleCollider2DComponent>());
                    if (ImGui::MenuItem("Sphere collider component"))
                        m_Entity.AddComponent<CircleCollider2DComponent>();
                    ImGui::EndDisabled();

                    ImGui::BeginDisabled(m_Entity.HasComponent<NativeScriptComponent>());
                    if (ImGui::MenuItem("Script component"))
                        m_Entity.AddComponent<NativeScriptComponent>();
                    ImGui::EndDisabled();

                    ImGui::EndPopup();
                }

                ImGui::EndTable();
            }

            glm::mat4 transform = m_Entity.GetComponent<TransformComponent>().GetTransform();
            auto& trs_component = m_Entity.GetComponent<TransformComponent>();

            ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 5.0f, 5.0f });
            if (ImGui::BeginTable("Properties", 2, ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_BordersInnerH)) {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("Translation");
                ImGui::SameLine();
                ImGui::TableNextColumn();
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                ImGui::DragFloat3("##floatT", glm::value_ptr((glm::vec3&)trs_component.Translation), 0.1f);
                // if (ImGui::DragFloat3("##floatT", glm::value_ptr(trs_component.Translation), 0.1f))
                //     transform = Utils::ComposeMatrix(trs_component.Translation, trs_component.Rotation, trs_component.Scale);

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("Rotation");
                ImGui::SameLine();
                ImGui::TableNextColumn();
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                ImGui::DragFloat3("##floatR", glm::value_ptr((glm::vec3&)trs_component.Rotation), 0.1f);
                // if (ImGui::DragFloat3("##floatR", glm::value_ptr(trs_component.rotation), 0.1f))
                //     transform = Utils::ComposeMatrix(trs_component.translation, trs_component.rotation, trs_component.scale);

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("Scale");
                ImGui::SameLine();
                ImGui::TableNextColumn();
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                ImGui::DragFloat3("##floatS", glm::value_ptr((glm::vec3&)trs_component.Scale), 0.01f);
                // if (ImGui::DragFloat3("##floatS", glm::value_ptr(trs_component.scale), 0.01f))
                //     transform = Utils::ComposeMatrix(trs_component.translation, trs_component.rotation, trs_component.scale);

                ImGui::EndTable();
            }
            ImGui::PopStyleVar();

            if (m_Entity.HasComponent<SpriteRendererComponent>()) {
                if (ImGui::Button(" - ##sprite_component")) {
                    m_Entity.RemoveComponent<SpriteRendererComponent>();
                } else {
                    ImGui::SameLine();
                    if (ImGui::TreeNode("Sprite component")) {
                        SpriteRendererComponent& sc = m_Entity.GetComponent<SpriteRendererComponent>();

                        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
                        ImGui::Text("Layer");
                        ImGui::SameLine();
                        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 5);
                        ImGui::DragInt("##", &sc.renderOrder, 0.05, 0, INT32_MAX);
                        ImGui::Separator();

                        ImGui::ColorPicker4("Color tint", (float*)&sc.Color,
                            ImGuiColorEditFlags_PickerHueWheel | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_DisplayRGB);

                        ImGui::Separator();

                        if (ImGui::BeginTable("properties_entity_name", 2, ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_BordersInnerV))
                            ;
                        {
                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            ImGui::Text("Material");

                            ImGui::TableNextColumn();

                            ImGui::SameLine();
                            if (ImGui::Button("Browse", { -FLT_MIN, 0.0f })) {
                                const char* filters[] = { "*.png", "*.jpg", "*.jpeg" };

                                std::string filepath = Tabby::FileDialogs::OpenFile(filters[0]).c_str();

                                if (filepath != "") {
                                    std::filesystem::path full_texture_path(filepath);
                                    std::filesystem::path texture_path = std::filesystem::current_path().append(full_texture_path.filename().string());

                                    std::filesystem::copy_file(filepath,
                                        texture_path,
                                        std::filesystem::copy_options::overwrite_existing);

                                    sc.Texture = AssetManager::Get()->LoadAssetSource(texture_path);

                                    // sc.Texture =

                                    // m_Context->GetRenderer()->AcquireTextureIndex(AssetManager::Get()->GetTexture(sc.texture), SamplerFilteringMode::NEAREST);
                                    // uvec3 texture_resolution = AssetManager::Get()->GetTexture(sc.texture)->GetSpecification().extent;
                                    // sc.aspect_ratio = { (float32)texture_resolution.x / (float32)texture_resolution.y };
                                }
                            };

                            ImGui::EndTable();
                        }
                        ImGui::TreePop();
                    }
                }
            }
            if (m_Entity.HasComponent<CameraComponent>()) {
                if (ImGui::Button(" - ##camera_component")) {
                    m_Entity.RemoveComponent<CameraComponent>();
                } else {
                    ImGui::SameLine();
                    if (ImGui::TreeNode("Camera component")) {

                        CameraComponent& camera_component = m_Entity.GetComponent<CameraComponent>();
                        Camera::ProjectionType projection_type = camera_component.Camera.GetProjectionType();

                        const char* items[] = { "Perspective", "Orthographic" };
                        static int32_t current_item = (int32_t)projection_type;

                        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 5.0f, 5.0f });
                        if (ImGui::BeginTable("Camera component properties table", 2, ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_BordersInnerH)) {
                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            ImGui::Text("Type");

                            ImGui::TableNextColumn();
                            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                            if (ImGui::BeginCombo("##combo", items[(int32_t)projection_type])) {
                                for (int n = 0; n < IM_ARRAYSIZE(items); n++) {
                                    bool is_selected = (current_item == n);
                                    if (ImGui::Selectable(items[n], is_selected, 0)) {
                                        current_item = n;

                                        switch (current_item) {
                                        case (int32_t)Camera::ProjectionType::Orthographic: {
                                            Camera& camera = camera_component.Camera;
                                            // camera_2D->SetProjection(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f);
                                            //
                                            // camera_2D->SetType(CameraProjectionType::PROJECTION_2D);
                                            // camera = ShareAs<Camera>(camera_2D);

                                            camera.SetProjectionType(Camera::ProjectionType::Orthographic);
                                            camera.SetOrthographic(10, -1, 1000);
                                            break;
                                        }
                                        case (int32_t)Camera::ProjectionType::Perspective: {
                                            Camera& camera = camera_component.Camera;
                                            // camera_2D->SetProjection(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f);
                                            //
                                            // camera_2D->SetType(CameraProjectionType::PROJECTION_2D);
                                            // camera = ShareAs<Camera>(camera_2D);

                                            camera.SetProjectionType(Camera::ProjectionType::Perspective);
                                            camera.SetPerspective(45.0f, 0.001f, 1000.0f);
                                            break;
                                        }

                                        default:
                                            break;
                                        }

                                        if (is_selected)
                                            ImGui::SetItemDefaultFocus();
                                    }
                                }
                                ImGui::EndCombo();
                            }

                            if (projection_type == Camera::ProjectionType::Perspective) {
                                // Shared<Camera3D> camera_3D = ShareAs<Camera3D>(camera_component.camera);

                                ImGui::TableNextRow();

                                ImGui::TableNextColumn();
                                ImGui::Text("Field of view");

                                ImGui::TableNextColumn();
                                static float32 fov = camera_component.Camera.GetPerspectiveVerticalFOV();
                                if (ImGui::SliderAngle("##", &fov, 30.0f, 160.0f))
                                    camera_component.Camera.SetPerspectiveVerticalFOV(fov);

                            } else if (projection_type == Camera::ProjectionType::Orthographic) {
                                ImGui::TableNextRow();

                                ImGui::TableNextColumn();
                                ImGui::Text("Scale");

                                ImGui::TableNextColumn();
                                static float32 orthographics_scale = camera_component.Camera.GetOrthographicSize();
                                if (ImGui::DragFloat("##", &orthographics_scale, 0.01f, 0.01f, FLT_MAX))
                                    camera_component.Camera.SetOrthographicSize(orthographics_scale);
                            }

                            ImGui::TableNextRow();

                            ImGui::TableNextColumn();
                            ImGui::Text("Primary");

                            ImGui::TableNextColumn();
                            ImGui::Checkbox("##checkbox_camera_primary", &camera_component.Primary);

                            ImGui::EndTable();
                        }
                        ImGui::PopStyleVar();
                        ImGui::TreePop();
                    }
                }
            }

            // Rigid body 2D
            if (m_Entity.HasComponent<Rigidbody2DComponent>()) {
                if (ImGui::Button(" - ##rb2d_component")) {
                    m_Entity.RemoveComponent<Rigidbody2DComponent>();
                } else {
                    ImGui::SameLine();
                    if (ImGui::TreeNode("Rigid body 2D component")) {
                        Rigidbody2DComponent& rb2d_component = m_Entity.GetComponent<Rigidbody2DComponent>();

                        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 5.0f, 5.0f });
                        if (ImGui::BeginTable("##rb2d_properties_table", 2, ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_BordersInnerH)) {
                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            ImGui::Text("Motion type");
                            ImGui::TableNextColumn();

                            const char* motion_type_strings[] = { "Static", "Dynamic", "Kinematic" };
                            if (ImGui::BeginCombo("##motion_type_combo", motion_type_strings[(int32_t)rb2d_component.Type])) {
                                for (int32_t i = 0; i < IM_ARRAYSIZE(motion_type_strings); i++) {
                                    bool selected = i == (int32_t)rb2d_component.Type;
                                    if (ImGui::Selectable(motion_type_strings[i], &selected))
                                        rb2d_component.SetBodyType((Rigidbody2DComponent::BodyType)i);
                                    // rb2d_component.Type = (Rigidbody2DComponent::BodyType)i;
                                }
                                ImGui::EndCombo();
                            }

                            // // Mass
                            // ImGui::TableNextRow();
                            // ImGui::TableNextColumn();
                            // ImGui::Text("Mass (kg)");
                            // ImGui::TableNextColumn();
                            // ImGui::DragFloat("##rb2d_mass_property", &rb2d_component.mass, 0.1f, 0.001f, FLT_MAX);
                            // if (rb2d_component.mass < 0.0f)
                            //     rb2d_component.mass = 0.001f;
                            //
                            // // Linear drag
                            // ImGui::TableNextRow();
                            // ImGui::TableNextColumn();
                            // ImGui::Text("Linear drag");
                            // ImGui::TableNextColumn();
                            // ImGui::DragFloat("##rb2d_linear_drag_property", &rb2d_component.linear_drag, 0.01f, 0.0f, 1.0f);
                            // if (rb2d_component.linear_drag < 0.0f)
                            //     rb2d_component.linear_drag = 0.0f;
                            //
                            // // Linear drag
                            // ImGui::TableNextRow();
                            // ImGui::TableNextColumn();
                            // ImGui::Text("Angular drag");
                            // ImGui::TableNextColumn();
                            // ImGui::DragFloat("##rb2d_angular_drag_property", &rb2d_component.angular_drag, 0.01f, 0.0f, 1.0f);
                            // if (rb2d_component.angular_drag < 0.0f)
                            //     rb2d_component.angular_drag = 0.0f;
                            //
                            // // Disable gravity
                            // ImGui::TableNextRow();
                            // ImGui::TableNextColumn();
                            // ImGui::Text("Disable gravity");
                            // ImGui::TableNextColumn();
                            // ImGui::Checkbox("##rb2d_disable_gravity_property", &rb2d_component.disable_gravity);
                            //
                            // // Sensor mode
                            // ImGui::TableNextRow();
                            // ImGui::TableNextColumn();
                            // ImGui::Text("Sensor mode");
                            // ImGui::TableNextColumn();
                            // ImGui::Checkbox("##rb2d_sensor_mode_property", &rb2d_component.sensor_mode);

                            // lock Z axis
                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            ImGui::Text("Lock Z rotation");
                            ImGui::TableNextColumn();
                            ImGui::Checkbox("##rb2d_z_lock_property", &rb2d_component.FixedRotation);

                            ImGui::EndTable();
                        }
                        ImGui::PopStyleVar();
                        ImGui::TreePop();
                    }
                }
            }
            if (m_Entity.HasComponent<BoxCollider2DComponent>()) {
                if (ImGui::Button(" - ##box_collider_component")) {
                    m_Entity.RemoveComponent<BoxCollider2DComponent>();
                } else {
                    ImGui::SameLine();
                    if (ImGui::TreeNode("Box collider component")) {
                        BoxCollider2DComponent& box_collider_component = m_Entity.GetComponent<BoxCollider2DComponent>();
                        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 5.0f, 5.0f });
                        if (ImGui::BeginTable("##box_collider_properties", 2, ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_BordersInnerH)) {
                            // Size
                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            ImGui::Text("Size");
                            ImGui::TableNextColumn();
                            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                            if (ImGui::DragFloat3("##box_collider_size_property", (float32*)&box_collider_component.Size, 0.01f, 0.01f, FLT_MAX)) {
                                if (box_collider_component.Size.x < 0.0f)
                                    box_collider_component.Size.x = 0.01f;
                                if (box_collider_component.Size.y < 0.0f)
                                    box_collider_component.Size.y = 0.01f;
                                // if (box_collider_component.Size.z < 0.0f)
                                //     box_collider_component.Size.z = 0.01f;
                            };

                            // // convex radius
                            // ImGui::TableNextRow();
                            // ImGui::TableNextColumn();
                            // ImGui::Text("Convex radius");
                            // ImGui::TableNextColumn();
                            // ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                            // ImGui::SliderFloat("##box_collider_convex_radius_property", &box_collider_component.convex_radius, 0.0f, 25.0f);

                            // Friction
                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            ImGui::Text("Friction");
                            ImGui::TableNextColumn();
                            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                            ImGui::SliderFloat("##box_collider_friction_property", &box_collider_component.Friction, 0.0f, 1.0f);

                            // Restitution
                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            ImGui::Text("Restitution");
                            ImGui::TableNextColumn();
                            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                            ImGui::SliderFloat("##box_collider_restitution_property", &box_collider_component.Restitution, 0.0f, 1.0f);

                            ImGui::EndTable();
                        }
                        ImGui::PopStyleVar();
                        ImGui::TreePop();
                    }
                }
            }
            if (m_Entity.HasComponent<CircleCollider2DComponent>()) {
                if (ImGui::Button(" - ##circle_collider_component")) {
                    m_Entity.RemoveComponent<CircleCollider2DComponent>();
                } else {
                    ImGui::SameLine();
                    if (ImGui::TreeNode("Circle collider component")) {
                        CircleCollider2DComponent& sphere_collider_component = m_Entity.GetComponent<CircleCollider2DComponent>();
                        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 5.0f, 5.0f });
                        if (ImGui::BeginTable("##sphere_collider_properties", 2, ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_BordersInnerH)) {
                            // Size
                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            ImGui::Text("Radius");
                            ImGui::TableNextColumn();
                            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                            if (ImGui::DragFloat("##sphere_collider_size_property", (float32*)&sphere_collider_component.Radius, 0.01f, 0.01f, FLT_MAX))
                                if (sphere_collider_component.Radius < 0.0f)
                                    sphere_collider_component.Radius = 0.01f;

                            // Friction
                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            ImGui::Text("Friction");
                            ImGui::TableNextColumn();
                            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                            ImGui::SliderFloat("##sphere_collider_friction_property", &sphere_collider_component.Friction, 0.0f, 1.0f);

                            // Restitution
                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            ImGui::Text("Restitution");
                            ImGui::TableNextColumn();
                            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                            ImGui::SliderFloat("##sphere_collider_restitution_property", &sphere_collider_component.Restitution, 0.0f, 1.0f);
                            ImGui::EndTable();
                        }
                        ImGui::PopStyleVar();
                        ImGui::TreePop();
                    }
                }
            }
            if (m_Entity.HasComponent<NativeScriptComponent>()) {
                if (ImGui::Button(" - ##native_script_component")) {
                    m_Entity.RemoveComponent<NativeScriptComponent>();
                } else {
                    ImGui::SameLine();
                    if (ImGui::TreeNode("Native Script component")) {
                        NativeScriptComponent& script_component = m_Entity.GetComponent<NativeScriptComponent>();
                        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 5.0f, 5.0f });

                        if (ImGui::BeginTable("##script_component_properties", 2, ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_BordersInnerH)) {
                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            ImGui::Text("Script name");

                            ImGui::TableNextColumn();
                            // ImGui::InputText("##", script_component.class_name.data(), script_component.class_name.capacity());
                            // script_component.class_name.resize(strlen(script_component.class_name.c_str()));
                            ImGui::EndTable();
                        }

                        ImGui::PopStyleVar();
                        ImGui::TreePop();
                    }
                }
            }
        }
        ImGui::End();
    }
}

void PropertiesPanel::SetEntity(Entity entity, bool selected)
{
    m_Entity = entity;
    m_Selected = selected;
}

}
