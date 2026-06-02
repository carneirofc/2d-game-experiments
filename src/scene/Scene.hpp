#pragma once
#include "world/World.hpp"
#include "scene/Level.hpp"
#include "scene/SpatialGrid.hpp"
#include "scene/LevelLoad.hpp"
#include "gfx/TextureCache.hpp"
#include "gfx/AnimationBank.hpp"
#include "systems/Movement.hpp"   // Physics
#include "systems/Input.hpp"      // PlayerConfig, PlayerAnims
#include <raylib.h>

namespace sc {

// Owns all gameplay state and runs the fixed-step system schedule. Knows nothing
// about windowing, ImGui, or the debug UI — those wrap the scene from outside.
struct Scene {
    World         world;
    Level         level;
    SpatialGrid   grid;
    Camera2D      cam{};
    TextureCache  textures;
    AnimationBank bank;

    Physics       phys{};
    PlayerConfig  player{};
    PlayerAnims   playerAnims{};
    int           crowdAnimId = -1;
    LevelSpawn    spawn{};               // parsed bootstrap parameters

    Entity        playerEntity = INVALID_ENTITY;
};

// Load assets + level, build the grid, create the player and the initial crowd.
void sceneInit(Scene& s, int screenW, int screenH,
               const char* manifestPath, const char* levelPath);

// One simulation tick: the documented system schedule (see Scene.cpp).
void sceneFixedUpdate(Scene& s, float dt);

// Draw the world + level into the camera. NO ImGui. outDrawn = entities drawn.
void sceneRender(Scene& s, int& outDrawn);

// Release GPU resources owned by the scene.
void sceneShutdown(Scene& s);

} // namespace sc
