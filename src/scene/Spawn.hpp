#pragma once
#include "scene/Scene.hpp"
#include <raylib.h>

namespace sc {

// Entity factory over a scene's world. Spawned entities use the scene's default
// crowd animation; the player gets FLAG_PLAYER and its idle animation.
Entity spawnPlayer(Scene& s, Vector2 at);

// Deterministic scatter of n crowd entities (index hash, no RNG).
void   spawnCrowd(Scene& s, int n);

// Destroy up to n non-player entities (handle compare skips the player).
void   despawnCrowd(Scene& s, int n);

} // namespace sc
