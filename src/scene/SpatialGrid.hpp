#pragma once
#include "scene/Level.hpp"
#include <raylib.h>
#include <cstdint>
#include <vector>

namespace sc {

// Uniform-grid broad-phase over the level's STATIC solids. Built once at load:
// each solid is bucketed into every cell its AABB overlaps. Collision then only
// tests the candidate solids in the cells an entity overlaps, turning the
// per-entity cost from O(allSolids) into O(fewLocalSolids).
//
// A per-query visit stamp de-duplicates solids that span multiple cells, so the
// callback fires at most once per solid per query without any per-call alloc.
struct SpatialGrid {
    float cellSize = 128.0f;
    int   cols = 0, rows = 0;
    float originX = 0.0f, originY = 0.0f;          // world coords of cell (0,0)
    std::vector<std::vector<std::uint32_t>> cells; // cell -> indices into Level.solids

    // Scratch for query de-dup; mutable so gridQuery can stamp during a const query.
    mutable std::vector<std::uint32_t> visitStamp;
    mutable std::uint32_t              queryEpoch = 0;
};

// Bucket the level's solids. Safe to call again if the level changes.
void gridBuild(SpatialGrid& g, const Level& lv, float cellSize = 128.0f);

// Invoke fn(solidIndex) once for each solid whose cell overlaps `box`.
template <class Fn>
void gridQuery(const SpatialGrid& g, Rectangle box, Fn&& fn) {
    if (g.cols <= 0 || g.rows <= 0) return;

    ++g.queryEpoch;
    auto cellX = [&](float x) {
        int c = static_cast<int>((x - g.originX) / g.cellSize);
        return c < 0 ? 0 : (c >= g.cols ? g.cols - 1 : c);
    };
    auto cellY = [&](float y) {
        int r = static_cast<int>((y - g.originY) / g.cellSize);
        return r < 0 ? 0 : (r >= g.rows ? g.rows - 1 : r);
    };
    int c0 = cellX(box.x),          r0 = cellY(box.y);
    int c1 = cellX(box.x + box.width), r1 = cellY(box.y + box.height);

    for (int r = r0; r <= r1; ++r) {
        for (int c = c0; c <= c1; ++c) {
            for (std::uint32_t si : g.cells[r * g.cols + c]) {
                if (g.visitStamp[si] == g.queryEpoch) continue; // already visited
                g.visitStamp[si] = g.queryEpoch;
                fn(si);
            }
        }
    }
}

} // namespace sc
