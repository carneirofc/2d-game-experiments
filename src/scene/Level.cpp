#include "scene/Level.hpp"

namespace sc {

void levelDraw(const Level& lv, Color color) {
    for (const Rectangle& r : lv.solids)
        DrawRectangleRec(r, color);
}

} // namespace sc
