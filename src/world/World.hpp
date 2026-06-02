#pragma once
#include "core/Types.hpp"
#include "world/Components.hpp"
#include <vector>
#include <cstdint>

namespace sc {

// Struct-of-arrays entity store. Component arrays are indexed by EntityIndex and
// stay parallel (sparse — may contain dead slots). A sparse-set (dense/denseSlot)
// gives systems a contiguous list of the live indices to iterate. Slots are
// recycled via a free-list; each slot carries a generation counter bumped on
// destroy so stale Entity handles can be detected. Plain data — systems read and
// write the arrays directly; the functions below manage allocation.
struct World {
    std::vector<std::uint32_t> flags;   // EntityFlags bitset
    std::vector<Vector2>       pos;
    std::vector<Vector2>       vel;
    std::vector<Vector2>       size;     // full w/h of the AABB
    std::vector<std::uint16_t> sheet;    // index into the sheet/texture table
    std::vector<AnimState>     anim;
    std::vector<Intent>        intent;   // controller intent (input/AI/network)
    std::vector<std::uint32_t> tint;     // packed 0xRRGGBBAA
    std::vector<Generation>    generation; // per-slot, bumped on destroy

    // Sparse-set for dense iteration: dense[0..aliveCount) holds the live
    // EntityIndex values contiguously; denseSlot[idx] is idx's position in
    // dense (or INVALID_INDEX when idx is dead).
    std::vector<EntityIndex>   dense;
    std::vector<std::uint32_t> denseSlot;

    std::vector<EntityIndex> freeList;
    std::size_t              aliveCount = 0;
};

void   worldInit(World& w, std::size_t reserve = 4096);
Entity worldCreate(World& w);
void   worldDestroy(World& w, Entity e);

// Resolve a handle to its current slot, or INVALID_INDEX if the handle is stale
// (slot recycled) or dead. The single validation point for stored references.
inline EntityIndex worldResolve(const World& w, Entity e) {
    if (e.index >= w.generation.size()) return INVALID_INDEX;
    if (w.generation[e.index] != e.gen) return INVALID_INDEX;
    if (!(w.flags[e.index] & FLAG_ACTIVE)) return INVALID_INDEX;
    return e.index;
}

inline bool worldAlive(const World& w, Entity e) {
    return worldResolve(w, e) != INVALID_INDEX;
}

// Build a handle for a raw slot (e.g. when iterating dense and needing to store
// a reference). The slot must currently be alive.
inline Entity worldHandle(const World& w, EntityIndex idx) {
    return Entity{idx, w.generation[idx]};
}

// Aliveness test for a raw slot (used by index-based hot loops).
inline bool worldAliveIndex(const World& w, EntityIndex idx) {
    return idx < w.flags.size() && (w.flags[idx] & FLAG_ACTIVE);
}

inline std::size_t worldCapacity(const World& w) { return w.flags.size(); }

} // namespace sc
