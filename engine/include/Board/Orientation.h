#pragma once

#include <iostream>
#include <set>

#include "Board/Direction.h"

namespace k10engine::Board {

enum class Orientation {
    Clock12Clock6,
    Clock2Clock8,
    Clock4Clock10,
};

Orientation get_orientation(Direction);
std::set<Direction> get_directions(Orientation);

std::ostream& operator<<(std::ostream&, Orientation);

} // namespace k10engine::Board
