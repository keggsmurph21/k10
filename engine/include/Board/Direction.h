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

static const Direction AllDirections[] = {
    Direction::Clock12, Direction::Clock2, Direction::Clock4,
    Direction::Clock6,  Direction::Clock8, Direction::Clock10,
};

Direction get_opposite(Direction);

} // namespace Board

} // namespace k10engine
