#include <Tabby/World/Map/Prefab.h>
#include <Tabby/World/Entity.h>

namespace Tabby {

static std::unordered_map<std::string, std::type_index> typeMap;

template <typename T>
void RegisterType()
{
    typeMap.emplace(typeid(T).name(), std::type_index(typeid(T)));
}

template <typename... Components>
void RegisterComponents(ComponentGroup<Components...>)
{
    (RegisterType<Components>(), ...);
}

void Prefab::InitializeTypeMap()
{
    RegisterComponents(AllComponents {});
}

std::vector<uint8_t> Prefab::SerializePrefab(Shared<Prefab> prefab)
{
    // std::vector<uint8_t> buffer;
    //
    // buffer = EntityData::SerializeEntityData(prefab->m_RootEntityData);
    //
    // return buffer;

    std::vector<uint8_t> buffer;
    // Serialize AssetHandle
    size_t handleSize = sizeof(prefab->Handle);
    buffer.resize(handleSize);
    std::memcpy(buffer.data(), &prefab->Handle, handleSize);

    // Serialize RootEntityData
    std::vector<uint8_t> entityData = EntityData::SerializeEntityData(prefab->m_RootEntityData);
    buffer.insert(buffer.end(), entityData.begin(), entityData.end());

    return buffer;
}

Shared<Prefab> Prefab::DeserializePrefab(AssetHandle handle, const std::vector<uint8_t>& data)
{
    // Shared<Prefab> prefab = CreateShared<Prefab>(UUID());
    //
    // if (data.size() < sizeof(prefab->m_RootEntityData)) {
    //     throw std::runtime_error("Insufficient data to deserialize Prefab");
    // }
    //
    // size_t offset = 0;
    // prefab->m_RootEntityData = EntityData::DeserializeEntityData(data, offset);
    //
    // return prefab;

    if (data.size() < sizeof(AssetHandle)) {
        throw std::runtime_error("Insufficient data to deserialize Prefab");
    }

    Shared<Prefab> prefab = CreateShared<Prefab>(handle);

    size_t offset = 0;

    // Deserialize AssetHandle
    std::memcpy(&prefab->Handle, data.data() + offset, sizeof(prefab->Handle));
    offset += sizeof(prefab->Handle);

    // Deserialize RootEntityData
    prefab->m_RootEntityData = EntityData::DeserializeEntityData(data, offset);

    return prefab;
}

void Prefab::SerializePrefabToFile(Shared<Prefab> prefab, const std::string& filePath)
{
    std::vector<uint8_t> buffer = SerializePrefab(prefab);

    std::ofstream outFile(filePath, std::ios::binary);
    if (!outFile) {
        throw std::runtime_error("Failed to open file for writing: " + filePath);
    }

    outFile.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());
    outFile.close();
}

Prefab::EntityData& Prefab::EntityData::AddChild()
{
    EntityData tempEntityData;
    tempEntityData.m_UUID = UUID();
    tempEntityData.m_Name = "PrefabChild";
    m_ChildMapping[tempEntityData.m_UUID] = tempEntityData;
    return m_ChildMapping[tempEntityData.m_UUID];
}

Prefab::EntityData& Prefab::GetRootEntity()
{
    return m_RootEntityData;
}

std::vector<uint8_t> Prefab::EntityData::SerializeEntityData(EntityData entityData)
{
    // Serialize UUID
    std::vector<uint8_t> serializedData;
    size_t uuidSize = sizeof(UUID);
    serializedData.resize(uuidSize);
    std::memcpy(serializedData.data(), &entityData.m_UUID, uuidSize);

    // Serialize name
    size_t nameSize = entityData.m_Name.size();
    serializedData.insert(serializedData.end(), reinterpret_cast<const uint8_t*>(&nameSize), reinterpret_cast<const uint8_t*>(&nameSize) + sizeof(size_t));
    serializedData.insert(serializedData.end(), entityData.m_Name.begin(), entityData.m_Name.end());

    // Serialize components
    size_t numComponents = entityData.m_Components.size();
    serializedData.insert(serializedData.end(), reinterpret_cast<const uint8_t*>(&numComponents), reinterpret_cast<const uint8_t*>(&numComponents) + sizeof(size_t));
    for (const auto& component : entityData.m_Components) {
        // Serialize component type
        std::string typeName = component.type.name();
        size_t typeNameSize = typeName.size();
        serializedData.insert(serializedData.end(), reinterpret_cast<const uint8_t*>(&typeNameSize), reinterpret_cast<const uint8_t*>(&typeNameSize) + sizeof(size_t));
        serializedData.insert(serializedData.end(), typeName.begin(), typeName.end());

        // Serialize component data size
        size_t componentDataSize = component.data.size();
        serializedData.insert(serializedData.end(), reinterpret_cast<const uint8_t*>(&componentDataSize), reinterpret_cast<const uint8_t*>(&componentDataSize) + sizeof(size_t));

        // Serialize component data
        serializedData.insert(serializedData.end(), component.data.begin(), component.data.end());
    }

    // Serialize children entities
    size_t numChildren = entityData.m_ChildMapping.size();
    serializedData.insert(serializedData.end(), reinterpret_cast<const uint8_t*>(&numChildren), reinterpret_cast<const uint8_t*>(&numChildren) + sizeof(size_t));
    for (const auto& childPair : entityData.m_ChildMapping) {
        // Serialize child entity
        std::vector<uint8_t> childSerializedData = SerializeEntityData(childPair.second);
        serializedData.insert(serializedData.end(), childSerializedData.begin(), childSerializedData.end());
    }

    return serializedData;
}

Prefab::EntityData Prefab::EntityData::DeserializeEntityData(const std::vector<uint8_t>& data, size_t& offset)
{
    EntityData entityData;

    // Deserialize UUID
    if (offset + sizeof(UUID) > data.size()) {
        throw std::runtime_error("Insufficient data for UUID");
    }
    std::memcpy(&entityData.m_UUID, data.data() + offset, sizeof(UUID));
    offset += sizeof(UUID);

    // Deserialize name size
    if (offset + sizeof(size_t) > data.size()) {
        throw std::runtime_error("Insufficient data for name size");
    }
    size_t nameSize;
    std::memcpy(&nameSize, data.data() + offset, sizeof(size_t));
    offset += sizeof(size_t);

    // Deserialize name
    if (offset + nameSize > data.size()) {
        throw std::runtime_error("Insufficient data for name");
    }
    entityData.m_Name.resize(nameSize);
    std::memcpy(entityData.m_Name.data(), data.data() + offset, nameSize);
    offset += nameSize;

    // Deserialize number of components
    if (offset + sizeof(size_t) > data.size()) {
        throw std::runtime_error("Insufficient data for number of components");
    }
    size_t numComponents;
    std::memcpy(&numComponents, data.data() + offset, sizeof(size_t));
    offset += sizeof(size_t);

    for (size_t i = 0; i < numComponents; ++i) {
        // Deserialize component type name size
        if (offset + sizeof(size_t) > data.size()) {
            throw std::runtime_error("Insufficient data for component type name size");
        }
        size_t typeNameSize;
        std::memcpy(&typeNameSize, data.data() + offset, sizeof(size_t));
        offset += sizeof(size_t);

        // Deserialize component type name
        if (offset + typeNameSize > data.size()) {
            throw std::runtime_error("Insufficient data for component type name");
        }
        std::string typeName(reinterpret_cast<const char*>(data.data() + offset), typeNameSize);
        offset += typeNameSize;

        auto it = typeMap.find(typeName);
        if (it == typeMap.end()) {
            throw std::runtime_error("Unknown component type: " + typeName);
        }
        std::type_index typeIndex = it->second;

        // Deserialize component data size
        if (offset + sizeof(size_t) > data.size()) {
            throw std::runtime_error("Insufficient data for component data size");
        }
        size_t componentDataSize;
        std::memcpy(&componentDataSize, data.data() + offset, sizeof(size_t));
        offset += sizeof(size_t);

        // Deserialize component data
        if (offset + componentDataSize > data.size()) {
            throw std::runtime_error("Insufficient data for component data");
        }
        std::vector<uint8_t> componentData(data.data() + offset, data.data() + offset + componentDataSize);
        offset += componentDataSize;

        entityData.m_Components.emplace_back(ComponentData { typeIndex, componentData });
    }

    // Deserialize number of children
    if (offset + sizeof(size_t) > data.size()) {
        throw std::runtime_error("Insufficient data for number of children");
    }
    size_t numChildren;
    std::memcpy(&numChildren, data.data() + offset, sizeof(size_t));
    offset += sizeof(size_t);

    for (size_t i = 0; i < numChildren; ++i) {
        EntityData childEntity = DeserializeEntityData(data, offset);
        entityData.m_ChildMapping.emplace(childEntity.m_UUID, childEntity);
    }

    return entityData;
}

Entity Prefab::Instantiate() const
{
    return CreateEntityFromPrefab(m_RootEntityData);
}

Entity Prefab::CreateEntityFromPrefab(EntityData entityData) const
{
    Entity newEntity = World::CreateEntityWithUUID(entityData.m_UUID, entityData.m_Name);

    for (auto& entData : entityData.m_ChildMapping) {
        Entity childEntity = CreateEntityFromPrefab(entData.second);
        newEntity.AddChild(childEntity);
    }

    for (const auto& componentData : entityData.m_Components) {
        if (componentData.type == typeid(IDComponent)) {
            auto deserializedComp = entityData.DeserializeComponent<IDComponent>(componentData.data);
            auto& createdComp = newEntity.AddOrReplaceComponent<IDComponent>();
            createdComp = deserializedComp;
            createdComp.ID = entityData.m_UUID;
        } else if (componentData.type == typeid(TagComponent)) {
            auto deserializedComp = entityData.DeserializeComponent<TagComponent>(componentData.data);
            auto& createdComp = newEntity.AddOrReplaceComponent<TagComponent>();
            createdComp = deserializedComp;
        } else if (componentData.type == typeid(HierarchyNodeComponent)) {
            auto deserializedComp = entityData.DeserializeComponent<HierarchyNodeComponent>(componentData.data);
            auto& createdComp = newEntity.AddOrReplaceComponent<HierarchyNodeComponent>();
            createdComp = deserializedComp;
        } else if (componentData.type == typeid(TransformComponent)) {
            auto deserializedComp = entityData.DeserializeComponent<TransformComponent>(componentData.data);
            auto& createdComp = newEntity.AddOrReplaceComponent<TransformComponent>();
            createdComp = deserializedComp;
        } else if (componentData.type == typeid(SpriteRendererComponent)) {
            auto deserializedComp = entityData.DeserializeComponent<SpriteRendererComponent>(componentData.data);
            auto& createdComp = newEntity.AddOrReplaceComponent<SpriteRendererComponent>();
            createdComp = deserializedComp;
        } else if (componentData.type == typeid(CircleRendererComponent)) {
            auto deserializedComp = entityData.DeserializeComponent<CircleRendererComponent>(componentData.data);
            auto& createdComp = newEntity.AddOrReplaceComponent<CircleRendererComponent>();
            createdComp = deserializedComp;
        } else if (componentData.type == typeid(CameraComponent)) {
            auto deserializedComp = entityData.DeserializeComponent<CameraComponent>(componentData.data);
            auto& createdComp = newEntity.AddOrReplaceComponent<CameraComponent>();
            createdComp = deserializedComp;
        } else if (componentData.type == typeid(SoundComponent)) {
            auto deserializedComp = entityData.DeserializeComponent<SoundComponent>(componentData.data);
            auto& createdComp = newEntity.AddOrReplaceComponent<SoundComponent>();
            createdComp = deserializedComp;
        } else if (componentData.type == typeid(NativeScriptComponent)) {
            auto deserializedComp = entityData.DeserializeComponent<NativeScriptComponent>(componentData.data);
            auto& createdComp = newEntity.AddOrReplaceComponent<NativeScriptComponent>();
            createdComp = deserializedComp;
        } else if (componentData.type == typeid(Rigidbody2DComponent)) {
            auto deserializedComp = entityData.DeserializeComponent<Rigidbody2DComponent>(componentData.data);
            auto& createdComp = newEntity.AddOrReplaceComponent<Rigidbody2DComponent>();
            createdComp = deserializedComp;
        } else if (componentData.type == typeid(BoxCollider2DComponent)) {
            auto deserializedComp = entityData.DeserializeComponent<BoxCollider2DComponent>(componentData.data);
            auto& createdComp = newEntity.AddOrReplaceComponent<BoxCollider2DComponent>();
            createdComp = deserializedComp;
        } else if (componentData.type == typeid(CircleCollider2DComponent)) {
            auto deserializedComp = entityData.DeserializeComponent<CircleCollider2DComponent>(componentData.data);
            auto& createdComp = newEntity.AddOrReplaceComponent<CircleCollider2DComponent>();
            createdComp = deserializedComp;
        } else if (componentData.type == typeid(CapsuleCollider2DComponent)) {
            auto deserializedComp = entityData.DeserializeComponent<CapsuleCollider2DComponent>(componentData.data);
            auto& createdComp = newEntity.AddOrReplaceComponent<CapsuleCollider2DComponent>();
            createdComp = deserializedComp;
        } else if (componentData.type == typeid(SegmentCollider2DComponent)) {
            auto deserializedComp = entityData.DeserializeComponent<SegmentCollider2DComponent>(componentData.data);
            auto& createdComp = newEntity.AddOrReplaceComponent<SegmentCollider2DComponent>();
            createdComp = deserializedComp;
        } else if (componentData.type == typeid(TextComponent)) {
            auto deserializedComp = entityData.DeserializeComponent<TextComponent>(componentData.data);
            auto& createdComp = newEntity.AddOrReplaceComponent<TextComponent>();
            createdComp = deserializedComp;
        }
    }

    return newEntity;
}

}
