# sidescroler

A 2D sprite-based sidescroller written from scratch in **C++23** — no game engine.
Built as a data-oriented (ECS-lite) sandbox aimed at thousands of entities at 60fps.

## What it is

- **Struct-of-arrays world.** Entities are indices; their components live in parallel
  arrays (`pos`, `vel`, `anim`, `intent`, …). No per-entity objects, no inheritance,
  no virtual dispatch in the hot loops.
- **Generational handles.** Stored references use `Entity{index, generation}`; a
  recycled slot can't alias a stale reference (`worldResolve` validates the generation).
- **Dense iteration.** A sparse-set lets systems iterate only the *live* entities, so a
  tick scales with population, not array capacity.
- **Spatial-grid collision.** The level's static solids are bucketed into a uniform grid
  once at load; each entity only tests nearby solids.
- **Data-driven.** Levels (`assets/levels/*.json`) and sprite sheets + animations
  (`assets/sprites/*.json` + `manifest.json`) load from disk — no recompile to tweak.
- **In-game tools.** ImGui debug HUD (F1) and a live sprite/animation editor (F2).

## Stack

- **C++23**, g++ 15.1.0 (MinGW) on Windows
- **raylib 5.5** — window / render / input / audio
- **Dear ImGui** + **rlImGui** — debug UI + sprite editor
- **nlohmann/json** — sprite/level sidecar files
- **CMake 4.x** + FetchContent — all deps fetched at configure time, no manual installs

## Build

```sh
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build -j
```

Binary: `build/sidescroler.exe`. Assets are copied to `build/assets/` post-build.

> First configure clones raylib / imgui / rlImGui / json — slow once, cached after.

### Build options

| Option | Default | Effect |
|--------|---------|--------|
| `-DSC_ENABLE_DEBUG=ON\|OFF` | `ON` | Compile in the ImGui HUD + sprite editor pass. `OFF` for a release-style build. |
| `-DCMAKE_BUILD_TYPE=Release\|Debug` | `Release` | Standard CMake build type. |

Example release build:
```sh
cmake -S . -B build -G "MinGW Makefiles" -DSC_ENABLE_DEBUG=OFF
cmake --build build -j
```

## Run

```sh
build/sidescroler.exe        # run from the repo root (assets resolve relative to cwd)
```

- **Move:** ← / → or A / D
- **Jump:** Space / W / ↑
- **F1:** toggle debug HUD (FPS, entity counts, spawn/despawn, physics tuning, inspector)
- **F2:** toggle sprite/animation editor (re-slice grid, edit anims, live preview, save to JSON)

## Layout

| Dir | Responsibility |
|-----|----------------|
| `src/core/`    | `App` (window + main loop + fixed timestep, driven by `AppCallbacks`), `Time`, `Types` (entity handle). Generic — no dependency on the layers below. |
| `src/world/`   | `World` struct-of-arrays (generational handles + sparse-set) and POD `Components`. |
| `src/scene/`   | `Scene` (owns gameplay state + the system schedule), `Level`, `SpatialGrid`, `LevelLoad`, `AssetManifest`, `Spawn`. |
| `src/systems/` | Plain functions over the arrays: `Input`, `Movement`, `Collision`, `Render`, `Animator`, `Camera`. |
| `src/gfx/`     | `TextureCache`, `SpriteSheet`, `AnimationDef` / `AnimationBank`. |
| `src/debug/`   | `DebugLayer` (injectable pass), `DebugUI`, `SpriteEditor`. |
| `src/main.cpp` | Composition root — wires `Scene` + `DebugLayer` into the `App` callbacks. |
| `assets/`      | `levels/*.json` (world bounds, solids, spawn, grid) and `sprites/` (PNGs, JSON sidecars, `manifest.json`). |

## Architecture in one breath

`main.cpp` builds a `Scene` and a `DebugLayer`, then hands the `App` five callbacks
(`init`, `onFrameStart`, `fixedUpdate`, `render`, `shutdown`). `App` owns the window and
the fixed-timestep loop and knows nothing about gameplay. Each tick, `sceneFixedUpdate`
runs the systems in a fixed order:

```
input -> control -> movement -> collision -> animator
```

Systems communicate only through the `World` arrays. The `Intent` component is the seam
between "who decides" (keyboard, AI, network) and "what happens" (locomotion, physics) —
see below.

## Extending: adding logic / AI to entities

Behavior plugs in by **writing the `Intent` component**, never by touching velocity or
position directly. `controlUpdate` turns intent into motion for *any* controlled entity,
so AI, networking, and replay all share one locomotion path.

1. **Add a flag** in `world/Components.hpp`: `FLAG_AI = 1u << 4`.
2. **Add a system** `systems/Ai.cpp` shaped like `inputUpdate` — loop the dense list, and
   for `FLAG_AI` entities write `intent[e].moveX` / `intent[e].jump` from whatever logic
   you want (chase the player, patrol, flee).
3. **Generalize control** — let `controlUpdate` process `FLAG_PLAYER | FLAG_AI` instead of
   player-only.
4. **Schedule it** in `sceneFixedUpdate`, right after `inputUpdate` (both produce intent
   before `controlUpdate` consumes it).

For **stateful** behavior (patrol → chase → flee, cooldowns), add an `AiState` POD
component to the `World` arrays (mode enum, timers, and an `Entity target` handle — the
generational check means a dead target stops being chased). The AI system then runs a
state machine, still emitting only `Intent`.

Rule of thumb: **AI reads the world and writes only its own `AiState` + `Intent`.**
Everything downstream (movement, collision, render) stays untouched.
