#pragma once
#include "world/World.hpp"
#include "gfx/AnimationBank.hpp"

namespace sc {

// Animation ids the player uses, resolved once at startup.
struct PlayerAnims {
    int idle = -1;
    int run  = -1;
    int jump = -1;
};

struct PlayerConfig {
    float moveSpeed = 220.0f; // px/s horizontal
    float jumpSpeed = 520.0f; // px/s initial upward
};

// Hardware -> Intent. Reads keyboard, writes intent[] for FLAG_PLAYER entities.
// No gameplay knowledge — could be swapped for AI/network without touching
// controlUpdate.
void inputUpdate(World& w);

// Intent -> effects. Consumes intent[] for FLAG_PLAYER entities: horizontal
// velocity, jump (only when grounded), facing flip, and animation selection.
void controlUpdate(World& w, const PlayerConfig& cfg, const PlayerAnims& anims);

} // namespace sc
