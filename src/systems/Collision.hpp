#pragma once
#include "world/World.hpp"
#include "scene/Level.hpp"
#include "scene/SpatialGrid.hpp"
#include <raylib.h>

namespace sc {

// Resolves every active entity's AABB against the level solids, using the grid
// broad-phase to test only nearby solids. Uses the smaller overlap axis to push
// out (so entities slide along surfaces and land cleanly), sets/clears
// FLAG_GROUNDED, and zeroes velocity on the contact axis.
void collisionUpdate(World& w, const Level& lv, const SpatialGrid& grid);

} // namespace sc
