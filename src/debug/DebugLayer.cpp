#include "debug/DebugLayer.hpp"
#include "scene/Spawn.hpp"
#include <rlImGui.h>
#include <raylib.h>

namespace sc {

void debugHotkeys(DebugLayer& d) {
    if (IsKeyPressed(KEY_F1)) d.dbg.showHud = !d.dbg.showHud;
    if (IsKeyPressed(KEY_F2)) d.dbg.showSpriteEditor = !d.dbg.showSpriteEditor;
}

void debugApplyRequests(DebugLayer& d, Scene& s) {
    if (d.dbg.spawnRequest > 0)   { spawnCrowd(s, d.dbg.spawnRequest);   d.dbg.spawnRequest = 0; }
    if (d.dbg.despawnRequest > 0) { despawnCrowd(s, d.dbg.despawnRequest); d.dbg.despawnRequest = 0; }
}

void debugDraw(DebugLayer& d, Scene& s, int drawn) {
    d.dbg.drawnLastFrame = drawn;
    rlImGuiBegin();
    debugHud(d.dbg, s.world, s.phys, s.player);
    spriteEditorDraw(d.editor, d.dbg.showSpriteEditor, s.bank, s.textures);
    rlImGuiEnd();
}

} // namespace sc
