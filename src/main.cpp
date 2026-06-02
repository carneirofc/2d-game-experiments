#include "core/App.hpp"
#include "scene/Scene.hpp"
#include "systems/Camera.hpp"
#ifdef SC_ENABLE_DEBUG
#include "debug/DebugLayer.hpp"
#include <rlImGui.h>
#endif
#include <raylib.h>

namespace {
constexpr const char* kManifest = "assets/sprites/manifest.json";
constexpr const char* kLevel    = "assets/levels/level0.json";
}

int main() {
    sc::Scene scene;
#ifdef SC_ENABLE_DEBUG
    sc::DebugLayer debug;
#endif

    sc::AppConfig cfg{1280, 720, "sidescroler"};
    sc::AppCallbacks cb;

    cb.init = [&] {
#ifdef SC_ENABLE_DEBUG
        rlImGuiSetup(true);
#endif
        sc::sceneInit(scene, cfg.width, cfg.height, kManifest, kLevel);
    };

    cb.onFrameStart = [&] {
#ifdef SC_ENABLE_DEBUG
        sc::debugHotkeys(debug);
        sc::debugApplyRequests(debug, scene);
#endif
    };

    cb.fixedUpdate = [&](float dt) { sc::sceneFixedUpdate(scene, dt); };

    cb.render = [&] {
        // Camera follows after the sim has advanced this frame.
        sc::cameraFollow(scene.cam, scene.world, scene.playerEntity,
                         GetScreenWidth(), GetScreenHeight());

        BeginDrawing();
        ClearBackground(Color{30, 32, 44, 255});
        int drawn = 0;
        sc::sceneRender(scene, drawn);
#ifdef SC_ENABLE_DEBUG
        sc::debugDraw(debug, scene, drawn);
#else
        (void)drawn;
#endif
        EndDrawing();
    };

    cb.shutdown = [&] {
        sc::sceneShutdown(scene);
#ifdef SC_ENABLE_DEBUG
        rlImGuiShutdown();
#endif
    };

    sc::appRun(cfg, cb);
    return 0;
}
