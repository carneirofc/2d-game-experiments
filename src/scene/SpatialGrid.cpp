#include "scene/SpatialGrid.hpp"
#include <algorithm>
#include <cmath>

namespace sc {

void gridBuild(SpatialGrid& g, const Level& lv, float cellSize) {
    g.cellSize = cellSize > 0.0f ? cellSize : 128.0f;

    // Bound the grid to the level's world extent, with a margin for the off-screen
    // side walls that sit at x = -32 and x = worldW.
    const float margin = g.cellSize;
    g.originX = -margin;
    g.originY = -margin;
    float spanW = lv.worldW + 2.0f * margin;
    float spanH = lv.worldH + 2.0f * margin;
    g.cols = std::max(1, static_cast<int>(std::ceil(spanW / g.cellSize)));
    g.rows = std::max(1, static_cast<int>(std::ceil(spanH / g.cellSize)));

    g.cells.assign(static_cast<std::size_t>(g.cols) * g.rows, {});
    g.visitStamp.assign(lv.solids.size(), 0);
    g.queryEpoch = 0;

    auto clampCol = [&](int c) { return c < 0 ? 0 : (c >= g.cols ? g.cols - 1 : c); };
    auto clampRow = [&](int r) { return r < 0 ? 0 : (r >= g.rows ? g.rows - 1 : r); };

    for (std::size_t i = 0; i < lv.solids.size(); ++i) {
        const Rectangle& s = lv.solids[i];
        int c0 = clampCol(static_cast<int>((s.x - g.originX) / g.cellSize));
        int r0 = clampRow(static_cast<int>((s.y - g.originY) / g.cellSize));
        int c1 = clampCol(static_cast<int>((s.x + s.width  - g.originX) / g.cellSize));
        int r1 = clampRow(static_cast<int>((s.y + s.height - g.originY) / g.cellSize));
        for (int r = r0; r <= r1; ++r)
            for (int c = c0; c <= c1; ++c)
                g.cells[r * g.cols + c].push_back(static_cast<std::uint32_t>(i));
    }
}

} // namespace sc
