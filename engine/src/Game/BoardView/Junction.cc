#include "Game/BoardView/Junction.h"
#include "Game/Player.h"

namespace k10engine::Game::BoardView {

std::ostream& operator<<(std::ostream& os, const Junction& junction)
{
    os << "BoardView::Junction{ " << junction.index() << ", ";
    if (junction.has_settlement()) {
        os << "(settlement: " << junction.owner()->index() << "), ";
    } else if (junction.has_city()) { // NOLINT(bugprone-branch-clone)
        os << "(city: " << junction.owner()->index() << "), ";
    } else if (junction.is_settleable()) {
        os << "(not yet settled), ";
    } else {
        os << "(not settleable), ";
    }
    if (junction.port_exchange_rate() == 0) {
        os << "(not a port) }";
    } else {
        os << "Port{ " << junction.port_exchange_rate() << ", " << junction.port_resources()
           << "} }";
    }
    return os;
}

} // namespace k10engine::Game::BoardView
