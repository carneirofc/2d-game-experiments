#pragma once
#include "debug/DebugUI.hpp"
#include "debug/SpriteEditor.hpp"
#include "scene/Scene.hpp"

namespace sc {

// Bundles the in-game debug tools (HUD + sprite editor) into one injectable
// layer. The layer observes the scene from outside the simulation: it reads
// post-tick state and pushes spawn/despawn requests back, but never inserts into
// the system schedule. Excluded from release builds via SC_ENABLE_DEBUG.
struct DebugLayer {
    DebugState   dbg{};
    SpriteEditor editor{};
};

// Toggle HUD (F1) and sprite editor (F2).
void debugHotkeys(DebugLayer& d);

// Apply queued spawn/despawn requests against the scene (call once per frame).
void debugApplyRequests(DebugLayer& d, Scene& s);

// Render the full debug pass (own rlImGuiBegin/End). drawn = entities drawn this
// frame, surfaced in the HUD.
void debugDraw(DebugLayer& d, Scene& s, int drawn);

} // namespace sc
