#pragma once

#include <tbpch.h>

#include <entt/entt.hpp>
#include <raylib.h>

namespace Tabby {

class Scene;

class Octree {
public:
    Octree(std::vector<entt::entity> GameObjects, float minNodeSize, Scene* Scene);
    // void Insert(const GameObject& object, const BoundingBox& objectBounds)
    // {
    //     root->Insert(object, objectBounds);
    // }
    //
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
        OctreeNode(const BoundingBox& bounds, float MinNodeSize, Vector3 BoundCenter, Vector3 BoundSize)
            : bound(bounds)
            , boundCenter(BoundCenter)
            , boundSize(BoundSize)
        {
            for (int i = 0; i < 8; i++) {
                children[i] = nullptr;
            }
        }

        // void Insert(const GameObject& object, const BoundingBox& objectBounds)
        // {
        // }

        // void Query(const BoundingBox& region, std::vector<GameObject>& result)
        // {
        // }

        void Draw()
        {
            DrawCubeWiresV(boundCenter, boundSize, GREEN);
        }

    private:
        BoundingBox bound;
        Vector3 boundCenter;
        Vector3 boundSize;
        float minSize;
        std::vector<entt::entity> objects;
        OctreeNode* children[8];
    };
    // -----------------------

    OctreeNode* root;
    Scene* m_Scene = nullptr;
};

}
