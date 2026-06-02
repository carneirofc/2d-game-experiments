#include "scene/Scene.hpp"
#include "scene/Spawn.hpp"
#include "scene/AssetManifest.hpp"
#include "systems/Collision.hpp"
#include "systems/Animator.hpp"
#include "systems/Render.hpp"
#include "systems/Camera.hpp"

namespace sc {

void sceneInit(Scene& s, int screenW, int screenH,
               const char* manifestPath, const char* levelPath) {
    worldInit(s.world, 8192);
    manifestLoad(s.bank, s.textures, manifestPath, s.playerAnims, s.crowdAnimId);
    levelLoad(s.level, s.spawn, levelPath);
    gridBuild(s.grid, s.level, s.spawn.cellSize);

    s.playerEntity = spawnPlayer(s, s.spawn.player);
    spawnCrowd(s, s.spawn.crowd);

    s.cam.zoom   = 1.0f;
    s.cam.offset = {screenW * 0.5f, screenH * 0.5f};
}

void sceneFixedUpdate(Scene& s, float dt) {
    // Schedule order is load-bearing:
    //   input   -> produces this step's Intent from hardware
    //   control -> consumes Intent into velocity / facing / animation request
    //   movement-> integrates velocity (gravity) into position
    //   collision-> corrects penetration, sets FLAG_GROUNDED (read by next tick's
    //               jump gate in control)
    //   animator-> advances frames over the now-resolved state
    inputUpdate(s.world);
    controlUpdate(s.world, s.player, s.playerAnims);
    movementUpdate(s.world, s.phys, dt);
    collisionUpdate(s.world, s.level, s.grid);
    animatorUpdate(s.world, s.bank, dt);
}

void sceneRender(Scene& s, int& outDrawn) {
    BeginMode2D(s.cam);
    levelDraw(s.level, Color{70, 74, 92, 255});
    outDrawn = renderEntities(s.world, s.bank, s.textures, s.cam);
    EndMode2D();
}

void sceneShutdown(Scene& s) {
    texUnloadAll(s.textures);
}

} // namespace sc
