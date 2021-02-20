#include <cassert>

#include "Board/Direction.h"

namespace k10engine::Board {

Direction get_opposite(Direction d)
{
    switch (d) {
    case Direction::Clock12:
        return Direction::Clock6;
    case Direction::Clock6:
        return Direction::Clock12;
    case Direction::Clock2:
        return Direction::Clock8;
    case Direction::Clock8:
        return Direction::Clock2;
    case Direction::Clock4:
        return Direction::Clock10;
    case Direction::Clock10:
        return Direction::Clock4;
    }
    assert(false);
}

std::ostream& operator<<(std::ostream& os, Direction d)
{
    switch (d) {
    case Direction::Clock2:
        os << "Direction::Clock2";
        break;
    case Direction::Clock4:
        os << "Direction::Clock4";
        break;
    case Direction::Clock6:
        os << "Direction::Clock6";
        break;
    case Direction::Clock8:
        os << "Direction::Clock8";
        break;
    case Direction::Clock10:
        os << "Direction::Clock10";
        break;
    case Direction::Clock12:
        os << "Direction::Clock12";
        break;
    }
    return os;
}

} // namespace k10engine::Board
