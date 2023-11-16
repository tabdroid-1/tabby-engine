// #include "raylib.h"
// #include <Math/Math.h>
#include <Scene/Components.h>
#include <Scene/GameObject.h>
#include <Scene/Octree/Octree.h>

namespace Tabby {

Octree::Octree(std::vector<entt::entity> entities, float minNodeSize, Scene* Scene)
    : m_Scene(Scene)
{

    BoundingBox bounds = { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } };

    for (auto& entity : entities) {

        auto& transform = m_Scene->m_Registry.get<TransformComponent>(entity);

        Vector3 halfSize = { transform.scale.x / 2.0f, transform.scale.y / 2.0f, transform.scale.z / 2.0f };

        Vector3 minPoint = { transform.position.x - halfSize.x, transform.position.y - halfSize.y, transform.position.z - halfSize.z };
        Vector3 maxPoint = { transform.position.x + halfSize.x, transform.position.y + halfSize.y, transform.position.z + halfSize.z };

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
    Vector3 boundsSize = Vector3Subtract(bounds.max, bounds.min);

    float maxSize = std::max(boundsSize.x, std::max(boundsSize.y, boundsSize.z));

    Vector3 boundSize = { maxSize, maxSize, maxSize };
    Vector3 boundCenter = Vector3Scale(Vector3Add(bounds.min, bounds.max), 0.5f);

    bounds.min = { boundCenter.x - boundSize.x, boundCenter.y - boundSize.y, boundCenter.z - boundSize.z };
    bounds.max = { boundCenter.x + boundSize.x, boundCenter.y + boundSize.y, boundCenter.z + boundSize.z };

    root = new OctreeNode(bounds, minNodeSize, boundCenter, boundSize);
}

}
