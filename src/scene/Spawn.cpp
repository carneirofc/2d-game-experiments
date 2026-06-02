#include "scene/Spawn.hpp"
#include "systems/Animator.hpp"   // playAnim

namespace sc {

static constexpr float kEntitySize = 32.0f; // default AABB; matches placeholder frame

Entity spawnPlayer(Scene& s, Vector2 at) {
    Entity e = worldCreate(s.world);
    EntityIndex i = e.index;
    s.world.flags[i] |= FLAG_PLAYER;
    s.world.size[i]   = {kEntitySize, kEntitySize};
    s.world.pos[i]    = at;
    if (s.playerAnims.idle >= 0) playAnim(s.world, i, s.playerAnims.idle);
    return e;
}

void spawnCrowd(Scene& s, int n) {
    for (int k = 0; k < n; ++k) {
        EntityIndex e = worldCreate(s.world).index;
        unsigned h = static_cast<unsigned>(e) * 2654435761u;
        s.world.pos[e]  = {60.0f + static_cast<float>(h % 5000),
                           40.0f + static_cast<float>((h >> 8) % 300)};
        s.world.vel[e]  = {static_cast<float>(static_cast<int>(h % 120) - 60), 0.0f};
        s.world.size[e] = {kEntitySize, kEntitySize};
        s.world.tint[e] = 0xFFFFFFFFu;
        if (s.crowdAnimId >= 0) {
            s.world.anim[e].animId   = static_cast<std::uint16_t>(s.crowdAnimId);
            s.world.anim[e].frameIdx = static_cast<std::uint16_t>(h % 4);
        }
    }
}

void despawnCrowd(Scene& s, int n) {
    EntityIndex playerIdx = worldResolve(s.world, s.playerEntity);
    int removed = 0;
    // Walk the dense list back-to-front: worldDestroy swap-removes the entry,
    // moving the last live entity into slot k — which we've already passed, so
    // each slot is visited exactly once.
    for (std::size_t k = s.world.aliveCount; k-- > 0 && removed < n;) {
        EntityIndex e = s.world.dense[k];
        if (e == playerIdx) continue;
        worldDestroy(s.world, worldHandle(s.world, e));
        ++removed;
    }
}

} // namespace sc
