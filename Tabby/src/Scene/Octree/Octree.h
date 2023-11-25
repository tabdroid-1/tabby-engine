#pragma once

#include <tbpch.h>

#include <entt/entt.hpp>
#include <raylib.h>
#include <raymath.h>

namespace Tabby {

class Scene;
class GameObject;

class Octree {
public:
    Octree(std::vector<entt::entity> entities, float minNodeSize, Scene* Scene);
    void Insert(const GameObject& object)
    {
        Insert(object);
    }

    void Insert(entt::entity object)
    {
        root->Insert(object);
    }

    std::vector<GameObject>& Query(const BoundingBox& region)
    {
        root->Query(region);
    }

    void Draw(Color Color)
    {
        root->Draw(Color);
    }

private:
    // ----- Octree Node -----

    class OctreeNode {
    public:
        OctreeNode(const BoundingBox& bounds, float MinNodeSize, Vector3 BoundCenter, Vector3 BoundSize, Scene* scene);
        void Insert(entt::entity object);
        std::vector<entt::entity>& Query(const BoundingBox& region);
        void Draw(Color Color);

    private:
        void DivideAndInsert(entt::entity object);

    private:
        BoundingBox bound;
        Vector3 boundCenter;
        Vector3 boundSize;
        float minSize;
        std::vector<entt::entity> objects;
        BoundingBox childBounds[8];
        Vector3 childBoundCenter[8];
        Vector3 childBoundSize;
        OctreeNode* children[8] { nullptr };
        Scene* m_Scene = nullptr;
    };
    // -----------------------

    OctreeNode* root;
    Scene* m_Scene = nullptr;
};
} // namespace Tabby
