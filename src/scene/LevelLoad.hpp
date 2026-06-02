#pragma once
#include "scene/Level.hpp"
#include <raylib.h>

namespace sc {

// Parsed bootstrap parameters that live alongside the geometry in a level file.
struct LevelSpawn {
    Vector2 player   = {100.0f, 100.0f};
    int     crowd    = 0;
    float   cellSize = 128.0f;
};

// Load a level's world bounds, solids, and spawn parameters from a JSON file.
// Returns false (and leaves outputs untouched) if the file can't be read/parsed.
bool levelLoad(Level& out, LevelSpawn& spawn, const char* path);

} // namespace sc
