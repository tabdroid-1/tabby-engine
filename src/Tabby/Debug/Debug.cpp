#include <Tabby/Debug/Debug.h>

namespace Tabby {

// void Debug::DrawRay(const glm::vec3& origin, const glm::vec3& direction, float length, const glm::vec4& color)
// {
//     TB_PROFILE_SCOPE_NAME("Tabby::Debug::DrawRay");
//
//     glm::vec3 dir = direction * length;
//     m_Lines.push_back({ origin, dir, color });
// }
// void Debug::DrawLine(const glm::vec3& point1, const glm::vec3& point2, const glm::vec4& color)
// {
//     TB_PROFILE_SCOPE_NAME("Tabby::Debug::DrawLine");
//
//     m_Lines.push_back({ point1, point2, color });
// }
// void Debug::DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
// {
//     TB_PROFILE_SCOPE_NAME("Tabby::Debug::DrawRect");
//
//     m_Rects.push_back({ position, size, color });
// }
// void Debug::DrawCircle(const glm::vec3& position, const glm::vec2& size, float thickness, const glm::vec4& color)
// {
//     TB_PROFILE_SCOPE_NAME("Tabby::Debug::DrawCircle");
//
//     m_Circles.push_back({ position, size, thickness, color });
// }
//
// void Debug::ProcessDrawCalls()
// {
//     TB_PROFILE_SCOPE_NAME("Tabby::Debug::ProcessDrawCalls");
//
//     for (auto& line : m_Lines) {
//         TB_PROFILE_SCOPE_NAME("Debug::ProcessDrawCalls::DrawLines");
//         Renderer2D::DrawLine(line.point1, line.point2, line.color, -1);
//         m_Lines.pop_back();
//     }
//
//     for (auto& rect : m_Rects) {
//         TB_PROFILE_SCOPE_NAME("Debug::ProcessDrawCalls::DrawRects");
//         Renderer2D::DrawRect(rect.position, rect.size, rect.color, -1);
//         m_Lines.pop_back();
//     }
//
//     for (auto& circle : m_Circles) {
//         TB_PROFILE_SCOPE_NAME("Debug::ProcessDrawCalls::DrawCircles");
//         Renderer2D::DrawCircle(circle.position, circle.size, 0.0f, circle.color, circle.thickness, 0.0f);
//         m_Lines.pop_back();
//     }
// }

}
