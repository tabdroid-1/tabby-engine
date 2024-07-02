#include <Tabby/World/Prefab.h>
#include <Tabby/World/Entity.h>

namespace Tabby {

template <typename T>
void Prefab::AddComponent(Entity& newEntity, const EntityData& entityData, const std::vector<uint8_t>& componentData)
{
    auto deserializedComp = entityData.DeserializeComponent<T>(componentData);
    auto& createdComp = newEntity.AddOrReplaceComponent<T>();
    createdComp = deserializedComp;
}

void Prefab::InitializeStandardTypes()
{
    RegisterComponents<TagComponent, HierarchyNodeComponent, TransformComponent, SpriteRendererComponent,
        CircleRendererComponent, CameraComponent, SoundComponent, Rigidbody2DComponent,
        BoxCollider2DComponent, CircleRendererComponent, CapsuleCollider2DComponent,
        SegmentCollider2DComponent, TextComponent>();
}

std::vector<uint8_t> Prefab::SerializePrefab(Shared<Prefab> prefab)
{
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

        auto it = m_TypeMap.find(typeName);
        if (it == m_TypeMap.end()) {
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
        } else {
            auto func = m_TypeAddComponentMap.find(componentData.type);
            func->second(newEntity, entityData, componentData.data);
        }
    }

    return newEntity;
}
}
