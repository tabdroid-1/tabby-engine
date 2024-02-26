#pragma once

#include "box2d/b2_world.h"
#include <Tabby/Core/Assert.h>
#include <Tabby/Scene/Entity.h>

namespace Tabby {

class Physics2DContactListener : public b2ContactListener {
public:
    Physics2DContactListener(b2World* world);

    void BeginContact(b2Contact* contact);
    void EndContact(b2Contact* contact);
    void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);
    void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse);
};

}
