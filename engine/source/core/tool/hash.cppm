export module crumb.core:tool.hash;

import std;

export namespace Crumb {

// ═══════════════════════════════════════════════════════════════
// hash_combine — 组合多个值的哈希到 seed
// ═══════════════════════════════════════════════════════════════

template<typename T>
inline void hash_combine(std::size_t& seed, const T& v)
{
    seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

template<typename T, typename... Ts>
inline void hash_combine(std::size_t& seed, const T& v, Ts... rest)
{
    hash_combine(seed, v);
    if constexpr (sizeof...(Ts) > 0)
    {
        hash_combine(seed, rest...);
    }
}

}// namespace Crumb