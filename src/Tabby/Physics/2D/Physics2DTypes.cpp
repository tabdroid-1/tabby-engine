#include <Tabby/Physics/2D/Physics2DTypes.h>
#include <Tabby/World/Components.h>

namespace Tabby {

// --- RaycastFilter2D ----------------

void RaycastFilter2D::SetCollisionLayer(uint32_t layer)
{
    TB_PROFILE_SCOPE_NAME("Tabby::RaycastFilter2D::SetCollisionLayer");

    collisionLayer = layer;
}

uint32_t RaycastFilter2D::GetCollisionLayer() const
{
    TB_PROFILE_SCOPE_NAME("Tabby::RaycastFilter2D::GetCollisionLayer");

    return collisionLayer;
}

void RaycastFilter2D::SetLayerValue(int layerNumber, bool value)
{
    TB_PROFILE_SCOPE_NAME("Tabby::RaycastFilter2D::SetLayerValue");

    TB_CORE_VERIFY_TAGGED(layerNumber >= 1, "Collision layer number must be between 1 and 32 inclusive.");
    TB_CORE_VERIFY_TAGGED(layerNumber <= 32, "Collision layer number must be between 1 and 32 inclusive.");
    if (value) {
        collisionLayer |= 1 << (layerNumber - 1);
    } else {
        collisionLayer &= ~(1 << (layerNumber - 1));
    }
}

bool RaycastFilter2D::GetLayerValue(int layerNumber) const
{
    TB_PROFILE_SCOPE_NAME("Tabby::RaycastFilter2D::GetLayerValue");

    TB_CORE_VERIFY_TAGGED(layerNumber >= 1, "Collision layer number must be between 1 and 32 inclusive.");
    TB_CORE_VERIFY_TAGGED(layerNumber <= 32, "Collision layer number must be between 1 and 32 inclusive.");
    return collisionLayer & (1 << (layerNumber - 1));
}

void RaycastFilter2D::SetCollisionMask(uint32_t layer)
{
    TB_PROFILE_SCOPE_NAME("Tabby::RaycastFilter2D::SetCollisionMask");

    collisionMask = layer;
}

uint32_t RaycastFilter2D::GetCollisionMask() const
{
    TB_PROFILE_SCOPE_NAME("Tabby::RaycastFilter2D::GetCollisionMask");

    return collisionMask;
}

void RaycastFilter2D::SetMaskValue(int layerNumber, bool value)
{
    TB_PROFILE_SCOPE_NAME("Tabby::RaycastFilter2D::SetMaskValue");

    TB_CORE_VERIFY_TAGGED(layerNumber >= 1, "Collision mask number must be between 1 and 32 inclusive.");
    TB_CORE_VERIFY_TAGGED(layerNumber <= 32, "Collision mask number must be between 1 and 32 inclusive.");
    if (value) {
        collisionMask |= 1 << (layerNumber - 1);
    } else {
        collisionMask &= ~(1 << (layerNumber - 1));
    }
}

bool RaycastFilter2D::GetMaskValue(int layerNumber) const
{
    TB_PROFILE_SCOPE_NAME("Tabby::RaycastFilter2D::GetMaskValue");

    TB_CORE_VERIFY_TAGGED(layerNumber >= 1, "Collision mask number must be between 1 and 32 inclusive.");
    TB_CORE_VERIFY_TAGGED(layerNumber <= 32, "Collision mask number must be between 1 and 32 inclusive.");
    return collisionMask & (1 << (layerNumber - 1));
}
// -------------------------------------
//
//
}
