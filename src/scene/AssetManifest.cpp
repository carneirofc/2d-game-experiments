#include "scene/AssetManifest.hpp"
#include <nlohmann/json.hpp>
#include <raylib.h>
#include <fstream>

using nlohmann::json;

namespace sc {

// --- Generate a placeholder sheet (PNG + JSON) so the demo runs unconfigured.
// Moved verbatim from the old Game.cpp bootstrap; driven by the manifest's
// "generateIfMissing" flag rather than hardcoded paths.
static void generatePlaceholder(const std::string& pngPath, const std::string& jsonPath) {
    if (FileExists(pngPath.c_str()) && FileExists(jsonPath.c_str())) return;

    constexpr int kFrame = 32, kCols = 4, kRows = 4;
    Image img = GenImageColor(kCols * kFrame, kRows * kFrame, BLANK);
    for (int r = 0; r < kRows; ++r) {
        for (int c = 0; c < kCols; ++c) {
            int idx = r * kCols + c;
            float h = (idx / static_cast<float>(kCols * kRows)) * 360.0f;
            Color body = ColorFromHSV(h, 0.6f, 0.9f);
            int x0 = c * kFrame, y0 = r * kFrame;
            ImageDrawRectangle(&img, x0 + 4, y0 + 4, kFrame - 8, kFrame - 8, body);
            int mx = x0 + 8 + (idx % 3) * 6;
            ImageDrawRectangle(&img, mx, y0 + 10, 5, 5, RAYWHITE);
        }
    }
    ExportImage(img, pngPath.c_str());
    UnloadImage(img);

    std::ofstream out(jsonPath);
    out << R"({
  "texture": "player.png",
  "grid": { "frameW": 32, "frameH": 32, "margin": 0, "spacing": 0 },
  "animations": {
    "idle": { "frames": [0, 1, 2, 3], "fps": 6, "loop": true },
    "run":  { "frames": [4, 5, 6, 7], "fps": 12, "loop": true },
    "jump": { "frames": [8, 9], "fps": 8, "loop": false }
  }
})" << '\n';
    TraceLog(LOG_INFO, "Generated placeholder assets at %s", pngPath.c_str());
}

// Derive the placeholder PNG path from a sheet's JSON path: same dir, .png ext.
static std::string pngFromJson(const std::string& jsonPath) {
    std::string p = jsonPath;
    if (auto dot = p.find_last_of('.'); dot != std::string::npos) p = p.substr(0, dot);
    return p + ".png";
}

bool manifestLoad(AnimationBank& bank, TextureCache& textures, const char* manifestPath,
                  PlayerAnims& outPlayer, int& outCrowdAnim) {
    std::ifstream f(manifestPath);
    if (!f) {
        TraceLog(LOG_WARNING, "manifestLoad: cannot open %s", manifestPath);
        return false;
    }
    json j;
    try {
        f >> j;
    } catch (const std::exception& e) {
        TraceLog(LOG_WARNING, "manifestLoad: parse error in %s: %s", manifestPath, e.what());
        return false;
    }

    std::string assetDir = j.value("assetDir", std::string{});

    if (j.contains("sheets")) {
        for (const auto& sh : j["sheets"]) {
            std::string name = sh.value("name", std::string{});
            std::string jsonPath = sh.value("json", std::string{});
            if (name.empty() || jsonPath.empty()) continue;
            if (sh.value("generateIfMissing", false))
                generatePlaceholder(pngFromJson(jsonPath), jsonPath);
            bankLoadSheet(bank, name, jsonPath, assetDir, textures);
        }
    }

    // Resolve named roles into concrete ids. Missing names resolve to -1.
    outPlayer = PlayerAnims{};
    outCrowdAnim = -1;
    if (j.contains("roles")) {
        const auto& roles = j["roles"];
        if (roles.contains("player")) {
            const auto& p = roles["player"];
            outPlayer.idle = bankAnimId(bank, p.value("idle", std::string{}));
            outPlayer.run  = bankAnimId(bank, p.value("run",  std::string{}));
            outPlayer.jump = bankAnimId(bank, p.value("jump", std::string{}));
        }
        if (roles.contains("crowd"))
            outCrowdAnim = bankAnimId(bank, roles["crowd"].value("idle", std::string{}));
    }

    return true;
}

} // namespace sc
