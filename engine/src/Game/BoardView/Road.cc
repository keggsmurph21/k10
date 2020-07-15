#include <cassert>

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

bool Road::operator==(const NodeView& other) const
{
    return node() == other.node() && type() == other.type() && owner() == static_cast<const Road&>(other).owner();
}

} // namespace k10engine::Game::BoardView

using Road = k10engine::Game::BoardView::Road;

template<>
void encode(ByteBuffer& buf, const Road& road)
{
    Encoder encoder(buf);
    if (road.owner() == nullptr) {
        encoder << false;
    } else {
        encoder << true;
        encoder << road.owner()->index();
    }
}
