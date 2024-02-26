#pragma once

#include "box2d/b2_world.h"
#include "box2d/box2d.h"
#include "glm/fwd.hpp"
#include <Tabby/Core/Assert.h>
#include <Tabby/Scene/Entity.h>
#include <atomic>

namespace Tabby {

struct RaycastHit2D;

class Physics2DRaycastCallback : public b2RayCastCallback {
public:
    Physics2DRaycastCallback();

    float ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction) override;
    void RefreshCallbackInfo(const glm::vec2& rayOrigin, RaycastHit2D* raycastHit);

private:
    glm::vec2 m_RayOrigin;
    RaycastHit2D* m_Raycasthit;
};

}
