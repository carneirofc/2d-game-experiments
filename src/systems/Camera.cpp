#include "systems/Camera.hpp"

namespace sc {

void cameraFollow(Camera2D& cam, const World& w, Entity target, int screenW, int screenH) {
    EntityIndex idx = worldResolve(w, target);
    if (idx == INVALID_INDEX) return;
    cam.target = {w.pos[idx].x, w.pos[idx].y};
    cam.offset = {screenW * 0.5f, screenH * 0.5f};
}

} // namespace sc
