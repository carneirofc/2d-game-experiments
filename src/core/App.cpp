#include "core/App.hpp"
#include <raylib.h>

namespace sc {

void appRun(const AppConfig& cfg, const AppCallbacks& cb) {
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(cfg.width, cfg.height, cfg.title);
    SetTargetFPS(0); // vsync paces; fixed-timestep handles sim

    if (cb.init) cb.init();

    FixedTimestep clock{};
    while (!WindowShouldClose()) {
        if (cb.onFrameStart) cb.onFrameStart();

        timeAdd(clock, GetFrameTime());
        while (timeConsume(clock))
            if (cb.fixedUpdate) cb.fixedUpdate(clock.step);

        if (cb.render) cb.render();
    }

    if (cb.shutdown) cb.shutdown();
    CloseWindow();
}

} // namespace sc
