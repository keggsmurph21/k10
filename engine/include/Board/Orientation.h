#pragma once

#include "Board/Direction.h"

namespace k10engine {

namespace Board {

enum class Orientation {
    Clock12Clock6,
    Clock2Clock8,
    Clock4Clock10,
};

Orientation get_orientation(Direction);

} // namespace Board

} // namespace k10engine
