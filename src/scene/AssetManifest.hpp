#pragma once
#include "gfx/AnimationBank.hpp"
#include "gfx/TextureCache.hpp"
#include "systems/Input.hpp"   // PlayerAnims

namespace sc {

// Loads every sheet listed in a sprite manifest into the bank, then resolves the
// named "roles" into concrete animation ids. Sheets flagged "generateIfMissing"
// get a placeholder PNG+JSON written first so the demo runs unconfigured.
//
// Fills outPlayer (idle/run/jump) and outCrowdAnim from the manifest's roles.
// Returns false if the manifest itself can't be read/parsed.
bool manifestLoad(AnimationBank& bank, TextureCache& textures, const char* manifestPath,
                  PlayerAnims& outPlayer, int& outCrowdAnim);

} // namespace sc
