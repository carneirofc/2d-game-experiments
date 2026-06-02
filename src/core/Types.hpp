#pragma once
#include <raylib.h>   // Vector2, Rectangle, Color, ...
#include <cstdint>
#include <limits>

namespace sc {

// Use raylib's own math/geometry types directly (Vector2, Rectangle, Color).
//
// Two notions of identity:
//   EntityIndex — raw slot into the SoA arrays. Used for hot iteration; never
//                 stored across frames (a recycled slot reuses the index).
//   Entity      — a stable handle (index + generation). Stored wherever a
//                 reference must outlive a frame (player, inspector, AI target).
//                 worldResolve() validates the generation before use, so a
//                 stale handle to a recycled slot resolves to INVALID_INDEX.
using EntityIndex = std::uint32_t;
using Generation  = std::uint32_t;

inline constexpr EntityIndex INVALID_INDEX = std::numeric_limits<EntityIndex>::max();

struct Entity {
    EntityIndex index = INVALID_INDEX;
    Generation  gen   = 0;
};

inline constexpr Entity INVALID_ENTITY{INVALID_INDEX, 0};

inline bool operator==(Entity a, Entity b) { return a.index == b.index && a.gen == b.gen; }
inline bool operator!=(Entity a, Entity b) { return !(a == b); }

} // namespace sc
