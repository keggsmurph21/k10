#include "Game/BoardView/Road.h"
#include "Game/Player.h"

namespace k10engine::Game::BoardView {

std::ostream& operator<<(std::ostream& os, const Road& road)
{
    os << "BoardView::Road{ " << road.index() << ", ";
    if (road.owner() == nullptr) {
        os << "(not owned) }";
    } else {
        os << "(owner: " << road.owner()->index() << ") }";
    }
    return os;
}

} // namespace k10engine::Game::BoardView
