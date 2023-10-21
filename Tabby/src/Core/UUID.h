#pragma once

// namespace Tabby {
//
// class UUID {
// public:
//     UUID();
//     UUID(uint64_t uuid);
//     UUID(const UUID&) = default;
//
//     operator uint64_t() const { return m_UUID; }
//
// private:
//     uint64_t m_UUID;
// };
// }
//
// template <>
// struct std::hash<Tabby::UUID> {
//     std::size_t operator()(const Tabby::UUID& uuid) const
//     {
//         return hash<uint64_t>()((uint64_t)uuid);
//     }
// };

#include <cstdint> // Include <cstdint>
#include <functional> // Include <functional>

namespace Tabby {

class UUID {
public:
    UUID();
    UUID(uint64_t uuid);
    UUID(const UUID&) = default;

    operator uint64_t() const { return m_UUID; }

private:
    uint64_t m_UUID;
};
}

namespace std {
template <>
struct hash<Tabby::UUID> {
    std::size_t operator()(const Tabby::UUID& uuid) const
    {
        return hash<uint64_t>()(static_cast<uint64_t>(uuid));
    }
};
}
