#include <cassert>

#include "Game/BoardView/Hex.h"

namespace k10engine::Game::BoardView {

std::ostream& operator<<(std::ostream& os, const Hex& hex)
{
    os << "BoardView::Hex{ " << hex.index() << ", " << hex.resource() << ", " << hex.roll_number() << " }";
    return os;
}

bool Hex::operator==(const NodeView& other) const
{
    return node() == other.node() && type() == other.type() && resource() == static_cast<const Hex&>(other).resource()
           && roll_number() == static_cast<const Hex&>(other).roll_number();
}

} // namespace k10engine::Game::BoardView
