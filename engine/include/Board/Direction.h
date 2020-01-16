#pragma once

namespace k10engine {

namespace Board {

enum class Direction {
    Clock12,
    Clock2,
    Clock4,
    Clock6,
    Clock8,
    Clock10,
};

Direction get_opposite(Direction);

} // namespace Board

} // namespace k10engine
