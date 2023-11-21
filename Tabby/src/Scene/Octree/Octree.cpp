// #include "raylib.h"
// #include <Math/Math.h>
#include <Scene/Components.h>
#include <Scene/GameObject.h>
#include <Scene/Octree/Octree.h>

namespace Tabby {

Octree::Octree(float minNodeSize, Color DrawColor, Scene* Scene)
    : m_Scene(Scene)
{

    BoundingBox bounds = { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } };

    for (auto& entity : m_Scene->m_Registry.view<TransformComponent>()) {
        auto transform = m_Scene->m_Registry.get<TransformComponent>(entity);
        if (m_Scene->m_Registry.any_of<BoxCollider2DComponent>(entity) && !(m_Scene->m_Registry.any_of<OctreeComponent>(entity) || m_Scene->m_Registry.any_of<CameraComponent>(entity))) {

            auto boxCollider = m_Scene->m_Registry.get<BoxCollider2DComponent>(entity);

            Vector3 halfSize = { boxCollider.Size.x + boxCollider.Offset.x, boxCollider.Size.y + boxCollider.Offset.x, 0.0f };

            Vector3 minPoint = { transform.Position.x - halfSize.x, transform.Position.y - halfSize.y, transform.Position.z - halfSize.z };
            Vector3 maxPoint = { transform.Position.x + halfSize.x, transform.Position.y + halfSize.y, transform.Position.z + halfSize.z };

            if (minPoint.x < bounds.min.x)
                bounds.min.x = minPoint.x;
            if (minPoint.y < bounds.min.y)
                bounds.min.y = minPoint.y;
            if (minPoint.z < bounds.min.z)
                bounds.min.z = minPoint.z;

            if (maxPoint.x > bounds.max.x)
                bounds.max.x = maxPoint.x;
            if (maxPoint.y > bounds.max.y)
                bounds.max.y = maxPoint.y;
            if (maxPoint.z > bounds.max.z)
                bounds.max.z = maxPoint.z;
        } else if (m_Scene->m_Registry.any_of<OctreeComponent>(entity) || m_Scene->m_Registry.any_of<CameraComponent>(entity)) {
            continue;
        } else {

            Vector3 halfSize = { transform.Scale.x / 2.0f, transform.Scale.y / 2.0f, transform.Scale.z / 2.0f };

            Vector3 minPoint = { transform.Position.x - halfSize.x, transform.Position.y - halfSize.y, transform.Position.z - halfSize.z };
            Vector3 maxPoint = { transform.Position.x + halfSize.x, transform.Position.y + halfSize.y, transform.Position.z + halfSize.z };

            if (minPoint.x < bounds.min.x)
                bounds.min.x = minPoint.x;
            if (minPoint.y < bounds.min.y)
                bounds.min.y = minPoint.y;
            if (minPoint.z < bounds.min.z)
                bounds.min.z = minPoint.z;

            if (maxPoint.x > bounds.max.x)
                bounds.max.x = maxPoint.x;
            if (maxPoint.y > bounds.max.y)
                bounds.max.y = maxPoint.y;
            if (maxPoint.z > bounds.max.z)
                bounds.max.z = maxPoint.z;
        }
    }
    Vector3 boundsSize = Vector3Subtract(bounds.max, bounds.min);

    float maxSize = std::max(boundsSize.x, std::max(boundsSize.y, boundsSize.z));

    Vector3 boundSize = { maxSize, maxSize, maxSize };
    Vector3 boundCenter = Vector3Scale(Vector3Add(bounds.min, bounds.max), 0.5f);

    bounds.min = { boundCenter.x - boundSize.x, boundCenter.y - boundSize.y, boundCenter.z - boundSize.z };
    bounds.max = { boundCenter.x + boundSize.x, boundCenter.y + boundSize.y, boundCenter.z + boundSize.z };

    root = new OctreeNode(bounds, minNodeSize, boundCenter, boundSize, DrawColor, m_Scene);

    for (auto entity : m_Scene->m_Registry.view<TransformComponent>()) {

        if (m_Scene->m_Registry.any_of<OctreeComponent>(entity) || m_Scene->m_Registry.any_of<CameraComponent>(entity)) {
            continue;
        }
        Insert(entity);
    }
}

Octree::OctreeNode::OctreeNode(const BoundingBox& bounds, float MinNodeSize, Vector3 BoundCenter, Vector3 BoundSize, Color DrawColor, Scene* scene)
    : bound(bounds)
    , boundCenter(BoundCenter)
    , boundSize(BoundSize)
    , minSize(MinNodeSize)
    , drawColor(DrawColor)
{

    m_Scene = scene;

    float quarter = boundSize.x / 4.0f;
    float childLenght = boundSize.x / 2.0f;
    childBoundSize = { childLenght, childLenght, childLenght };

    Vector3 childSizeMin = Vector3Add(Vector3Subtract(BoundCenter, childBoundSize), { -quarter, quarter, -quarter });
    Vector3 childSizeMax = Vector3Add(Vector3Add(BoundCenter, childBoundSize), { -quarter, quarter, -quarter });
    childBounds[0] = { childSizeMin, childSizeMax };
    childBoundCenter[0] = Vector3Scale(Vector3Add(childSizeMin, childSizeMax), 0.5f);

    childSizeMin = Vector3Add(Vector3Subtract(BoundCenter, childBoundSize), { quarter, quarter, -quarter });
    childSizeMax = Vector3Add(Vector3Add(BoundCenter, childBoundSize), { quarter, quarter, -quarter });
    childBounds[1] = { childSizeMin, childSizeMax };
    childBoundCenter[1] = Vector3Scale(Vector3Add(childSizeMin, childSizeMax), 0.5f);

    childSizeMin = Vector3Add(Vector3Subtract(BoundCenter, childBoundSize), { -quarter, quarter, quarter });
    childSizeMax = Vector3Add(Vector3Add(BoundCenter, childBoundSize), { -quarter, quarter, quarter });
    childBounds[2] = { childSizeMin, childSizeMax };
    childBoundCenter[2] = Vector3Scale(Vector3Add(childSizeMin, childSizeMax), 0.5f);

    childSizeMin = Vector3Add(Vector3Subtract(BoundCenter, childBoundSize), { quarter, quarter, quarter });
    childSizeMax = Vector3Add(Vector3Add(BoundCenter, childBoundSize), { quarter, quarter, quarter });
    childBounds[3] = { childSizeMin, childSizeMax };
    childBoundCenter[3] = Vector3Scale(Vector3Add(childSizeMin, childSizeMax), 0.5f);

    childSizeMin = Vector3Add(Vector3Subtract(BoundCenter, childBoundSize), { -quarter, -quarter, -quarter });
    childSizeMax = Vector3Add(Vector3Add(BoundCenter, childBoundSize), { -quarter, -quarter, -quarter });
    childBounds[4] = { childSizeMin, childSizeMax };
    childBoundCenter[4] = Vector3Scale(Vector3Add(childSizeMin, childSizeMax), 0.5f);

    childSizeMin = Vector3Add(Vector3Subtract(BoundCenter, childBoundSize), { quarter, -quarter, -quarter });
    childSizeMax = Vector3Add(Vector3Add(BoundCenter, childBoundSize), { quarter, -quarter, -quarter });
    childBounds[5] = { childSizeMin, childSizeMax };
    childBoundCenter[5] = Vector3Scale(Vector3Add(childSizeMin, childSizeMax), 0.5f);

    childSizeMin = Vector3Add(Vector3Subtract(BoundCenter, childBoundSize), { -quarter, -quarter, quarter });
    childSizeMax = Vector3Add(Vector3Add(BoundCenter, childBoundSize), { -quarter, -quarter, quarter });
    childBounds[6] = { childSizeMin, childSizeMax };
    childBoundCenter[6] = Vector3Scale(Vector3Add(childSizeMin, childSizeMax), 0.5f);

    childSizeMin = Vector3Add(Vector3Subtract(BoundCenter, childBoundSize), { quarter, -quarter, quarter });
    childSizeMax = Vector3Add(Vector3Add(BoundCenter, childBoundSize), { quarter, -quarter, quarter });
    childBounds[7] = { childSizeMin, childSizeMax };
    childBoundCenter[7] = Vector3Scale(Vector3Add(childSizeMin, childSizeMax), 0.5f);
}

void Octree::OctreeNode::Insert(entt::entity object)
{
    DivideAndInsert(object);
}

void Octree::OctreeNode::DivideAndInsert(entt::entity object)
{
    if (boundSize.y <= minSize) {
        return;
    }

    bool dividing = false;
    for (int i = 0; i < 8; i++) {
        if (children[i] == nullptr) {
            children[i] = new OctreeNode(childBounds[i], minSize, childBoundCenter[i], childBoundSize, drawColor, m_Scene);
        }

        BoundingBox objectBound;

        auto transform = m_Scene->m_Registry.get<TransformComponent>(object);
        if (m_Scene->m_Registry.any_of<BoxCollider2DComponent>(object)) {

            auto boxCollider = m_Scene->m_Registry.get<BoxCollider2DComponent>(object);

            Vector3 halfSize = { boxCollider.Size.x + boxCollider.Offset.x, boxCollider.Size.y + boxCollider.Offset.x, 0.0f };

            Vector3 minPoint = { transform.Position.x - halfSize.x, transform.Position.y - halfSize.y, transform.Position.z - halfSize.z };
            Vector3 maxPoint = { transform.Position.x + halfSize.x, transform.Position.y + halfSize.y, transform.Position.z + halfSize.z };

            objectBound = { minPoint, maxPoint };
        }
        if (m_Scene->m_Registry.any_of<SpriteRendererComponent>(object) && !m_Scene->m_Registry.any_of<BoxCollider2DComponent>(object)) {

            Vector3 halfSize = { transform.Scale.x / 2.0f, transform.Scale.y / 2.0f, 0.0f };

            Vector3 minPoint = { transform.Position.x - halfSize.x, transform.Position.y - halfSize.y, transform.Position.z - halfSize.z };
            Vector3 maxPoint = { transform.Position.x + halfSize.x, transform.Position.y + halfSize.y, transform.Position.z + halfSize.z };

            objectBound = { minPoint, maxPoint };

        } else {

            Vector3 halfSize = { transform.Scale.x / 2.0f, transform.Scale.y / 2.0f, transform.Scale.z / 2.0f };

            Vector3 minPoint = { transform.Position.x - halfSize.x, transform.Position.y - halfSize.y, transform.Position.z - halfSize.z };
            Vector3 maxPoint = { transform.Position.x + halfSize.x, transform.Position.y + halfSize.y, transform.Position.z + halfSize.z };

            objectBound = { minPoint, maxPoint };
        }

        if (CheckCollisionBoxes(childBounds[i], objectBound)) {
            dividing = true;
            children[i]->DivideAndInsert(object);
        }
    }

    if (!dividing) {
        for (int i = 0; i < 8; ++i) {
            delete children[i];
            children[i] = nullptr;
        }
    }
}

void Octree::OctreeNode::Draw()
{
    DrawCubeWiresV(boundCenter, boundSize, drawColor);

    if (children != nullptr) {
        for (auto child : children) {
            if (child != nullptr) {
                child->Draw();
            }
        }
    }
}
}
