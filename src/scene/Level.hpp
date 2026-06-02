#pragma once
#include <raylib.h>
#include <vector>

namespace sc {

// The level is just a list of solid rectangles in world pixels. Owned by the
// scene. Collision resolves entity AABBs against these (broad-phased by the
// SpatialGrid); rendering draws them for visualization.
struct Level {
    std::vector<Rectangle> solids;
    float worldW = 0.0f;
    float worldH = 0.0f;
};

inline void levelAddSolid(Level& lv, Rectangle r) { lv.solids.push_back(r); }

// Draw the solids (debug / level visualization).
void levelDraw(const Level& lv, Color color);

} // namespace sc
