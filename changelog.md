Version: 01-04-2024_1 Alpha
Changes:
- Some Hierarchy rework
  - You can now properly remove entities.
    - DestroyEntity(Entity entity): This will delete entity but not children. If entity has parent its childrens parent will be the entitys parent before finaly destroyed.
    - DestroyEntityWithChildren(Entity entity): This will delete entity and its children. 
  - Data is held in HierarchyNodeComponent instead of Transform component.

