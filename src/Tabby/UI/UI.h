#pragma once
#include <imgui/imgui.h>

namespace Tabby::UI {

struct ScopedStyleColor {
    ScopedStyleColor() = default;

    ScopedStyleColor(ImGuiCol idx, ImVec4 color, bool predicate = true)
        : m_Set(predicate)
    {
        TB_PROFILE_SCOPE_NAME("Tabby::ScopedStyleColor::Constructor");

        if (predicate)
            ImGui::PushStyleColor(idx, color);
    }

    ScopedStyleColor(ImGuiCol idx, ImU32 color, bool predicate = true)
        : m_Set(predicate)
    {
        TB_PROFILE_SCOPE_NAME("Tabby::ScopedStyleColor::Constructor");
        if (predicate)
            ImGui::PushStyleColor(idx, color);
    }

    ~ScopedStyleColor()
    {
        TB_PROFILE_SCOPE_NAME("Tabby::ScopedStyleColor::Destructors");
        if (m_Set)
            ImGui::PopStyleColor();
    }

private:
    bool m_Set = false;
};

}
