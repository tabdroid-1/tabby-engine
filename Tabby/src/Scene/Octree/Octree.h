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
    Octree(float minNodeSize, Color DrawColor, Scene* Scene);
    void Insert(const GameObject& object)
    {
        Insert(object);
    }

    void Insert(entt::entity object)
    {
        root->Insert(object);
    }

    // void Query(const BoundingBox& region, std::vector<GameObject>& result)
    // {
    //     root->Query(region, result);
    // }

    void Draw()
    {
        root->Draw();
    }

private:
    // ----- Octree Node -----

    class OctreeNode {
    public:
        OctreeNode(const BoundingBox& bounds, float MinNodeSize, Vector3 BoundCenter, Vector3 BoundSize, Color DrawColor, Scene* scene);
        void Insert(entt::entity object);
        // void Query(const BoundingBox& region, std::vector<GameObject>& result)
        // {
        // }

        void Draw();

    private:
        void DivideAndInsert(entt::entity object);

    private:
        BoundingBox bound;
        Vector3 boundCenter;
        Vector3 boundSize;
        float minSize;
        std::vector<entt::entity> objects;
        Color drawColor = GREEN;
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

}
