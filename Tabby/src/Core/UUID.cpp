#include <Core/UUID.h>
#include <tbpch.h>

namespace Tabby {

static std::unordered_map<UUID, std::string> m_map;

static void AddToMap()
{
    m_map[UUID()] = "tabby";
}

static std::random_device s_RandomDevice;
static std::mt19937_64 s_Engine(s_RandomDevice());
static std::uniform_int_distribution<uint32_t> s_UniformDistribution;

UUID::UUID()
    : m_UUID(s_UniformDistribution(s_Engine))
{
}

UUID::UUID(uint64_t uuid)
    : m_UUID(uuid)
{
}

}
