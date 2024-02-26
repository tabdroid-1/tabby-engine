#include "Physics2DRaycastCallback.h"
#include "Physics2D.h"

namespace Tabby {

Physics2DRaycastCallback::Physics2DRaycastCallback()
{
}

float Physics2DRaycastCallback::ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction)
{
    m_Raycasthit->point = { point.x, point.y };
    m_Raycasthit->normal = { normal.x, normal.y };
    m_Raycasthit->fraction = fraction;

    b2BodyUserData bodyUserData = fixture->GetBody()->GetUserData();

    Entity e;

    e = { static_cast<entt::entity>(bodyUserData.pointer) };

    m_Raycasthit->entity = e;
    m_Raycasthit->transform = &e.GetComponent<TransformComponent>();
    m_Raycasthit->rigidbody = &e.GetComponent<Rigidbody2DComponent>();
    m_Raycasthit->distance = glm::distance(m_RayOrigin, { point.x, point.y });

    return fraction;
}

void Physics2DRaycastCallback::RefreshCallbackInfo(const glm::vec2& rayOrigin, RaycastHit2D* raycastHit)
{
    m_Raycasthit = raycastHit;
    m_RayOrigin = rayOrigin;
}
}
