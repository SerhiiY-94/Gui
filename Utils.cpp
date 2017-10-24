#include "Utils.h"

math::vec2 ui::MapPointToScreen(const math::ivec2 &p, const math::ivec2 &res) {
    return (2.0f * math::vec2((float)p.x, (float)res.y - p.y)) / (math::vec2)res + math::vec2(-1, -1);
}

