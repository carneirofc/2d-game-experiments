#include "world/World.hpp"

namespace sc {

static void worldGrow(World& w, std::size_t n) {
    w.flags.resize(n, FLAG_NONE);
    w.pos.resize(n);
    w.vel.resize(n);
    w.size.resize(n, Vector2{16.0f, 16.0f});
    w.sheet.resize(n, 0);
    w.anim.resize(n);
    w.intent.resize(n);
    w.tint.resize(n, 0xFFFFFFFFu);
    w.generation.resize(n, 0);
    w.denseSlot.resize(n, INVALID_INDEX);
}

void worldInit(World& w, std::size_t reserve) {
    w.flags.reserve(reserve);
    w.pos.reserve(reserve);
    w.vel.reserve(reserve);
    w.size.reserve(reserve);
    w.sheet.reserve(reserve);
    w.anim.reserve(reserve);
    w.intent.reserve(reserve);
    w.tint.reserve(reserve);
    w.generation.reserve(reserve);
    w.dense.reserve(reserve);
    w.denseSlot.reserve(reserve);
    w.freeList.reserve(reserve);
}

Entity worldCreate(World& w) {
    EntityIndex e;
    if (!w.freeList.empty()) {
        e = w.freeList.back();
        w.freeList.pop_back();
    } else {
        e = static_cast<EntityIndex>(w.flags.size());
        worldGrow(w, w.flags.size() + 1);
    }
    w.flags[e]  = FLAG_ACTIVE;
    w.pos[e]    = {};
    w.vel[e]    = {};
    w.size[e]   = {16.0f, 16.0f};
    w.sheet[e]  = 0;
    w.anim[e]   = {};
    w.intent[e] = {};
    w.tint[e]   = 0xFFFFFFFFu;

    // Append to the dense list.
    w.denseSlot[e] = static_cast<std::uint32_t>(w.aliveCount);
    if (w.aliveCount < w.dense.size()) w.dense[w.aliveCount] = e;
    else                               w.dense.push_back(e);
    ++w.aliveCount;

    return Entity{e, w.generation[e]};
}

void worldDestroy(World& w, Entity handle) {
    EntityIndex e = worldResolve(w, handle);
    if (e == INVALID_INDEX) return; // stale or already dead — double-free safe

    // Swap-remove from the dense list to keep [0,aliveCount) contiguous.
    std::uint32_t slot = w.denseSlot[e];
    std::uint32_t last = static_cast<std::uint32_t>(w.aliveCount - 1);
    EntityIndex   lastEntity = w.dense[last];
    w.dense[slot]          = lastEntity;
    w.denseSlot[lastEntity] = slot;
    w.denseSlot[e]         = INVALID_INDEX;
    --w.aliveCount;

    w.flags[e] = FLAG_NONE;
    ++w.generation[e]; // invalidate any stored handle to this slot
    w.freeList.push_back(e);
}

} // namespace sc
