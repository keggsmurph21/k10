#pragma once

#include <set>

#include "Board/Direction.h"

namespace k10engine {

namespace Board {

enum class Orientation {
    Clock12Clock6,
    Clock2Clock8,
    Clock4Clock10,
};

Orientation get_orientation(Direction);
const std::set<Direction> get_directions(Orientation);

} // namespace Board

} // namespace k10engine
