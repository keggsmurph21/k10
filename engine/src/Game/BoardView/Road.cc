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
    return node() == other.node() && type() == other.type()
           && owner() == static_cast<const Road&>(other).owner();
}

std::vector<u8> Road::serialize() const
{
    return { static_cast<u8>(type()) };
}

Road Road::deserialize(const Board::Node& node, const std::vector<u8>& serial)
{
    size_t index = 0;
    // NOLINTNEXTLINE(bugprone-assert-side-effect)
    assert(static_cast<NodeView::Type>(serial[index++]) == NodeView::Type::Road);
    return Road{ node };
}

} // namespace k10engine::Game::BoardView
