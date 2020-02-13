#include <stdexcept>

#include "Board/Orientation.h"

namespace k10engine::Board {

Orientation get_orientation(Direction d)
{
    switch (d) {
    case Direction::Clock12:
    case Direction::Clock6:
        return Orientation::Clock12Clock6;
    case Direction::Clock2:
    case Direction::Clock8:
        return Orientation::Clock2Clock8;
    case Direction::Clock4:
    case Direction::Clock10:
        return Orientation::Clock4Clock10;
    default:
        throw std::invalid_argument("Unrecognized Direction");
    }
}

std::set<Direction> get_directions(Orientation o)
{
    switch (o) {
    case Orientation::Clock12Clock6:
        return { Direction::Clock12, Direction::Clock6 };
    case Orientation::Clock2Clock8:
        return { Direction::Clock2, Direction::Clock8 };
    case Orientation::Clock4Clock10:
        return { Direction::Clock4, Direction::Clock10 };
    default:
        throw std::invalid_argument("Unrecognized Orientation");
    }
}

std::ostream& operator<<(std::ostream& os, Orientation o)
{
    switch (o) {
    case Orientation::Clock12Clock6:
        os << "Orientation::Clock12Clock6";
        break;
    case Orientation::Clock2Clock8:
        os << "Orientation::Clock2Clock8";
        break;
    case Orientation::Clock4Clock10:
        os << "Orientation::Clock4Clock10";
        break;
    default:
        throw std::invalid_argument("Unrecognized Orientation");
    }
    return os;
}

} // namespace k10engine::Board
