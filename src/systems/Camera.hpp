#pragma once
#include "world/World.hpp"
#include <raylib.h>

namespace sc {

// Centers the camera on the target entity (if the handle still resolves) and
// keeps the view offset at screen center. No-op when the target is gone, so the
// camera holds its last position instead of snapping to a recycled slot.
void cameraFollow(Camera2D& cam, const World& w, Entity target, int screenW, int screenH);

} // namespace sc
