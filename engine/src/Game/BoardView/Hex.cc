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

std::vector<u8> Hex::serialize() const
{
    return { static_cast<u8>(type()),
             static_cast<u8>(::k10engine::serialize(resource())),
             static_cast<u8>(roll_number()) };
}

Hex Hex::deserialize(const Board::Node& node, const std::vector<u8>& serial)
{
    size_t index = 0;
    // NOLINTNEXTLINE(bugprone-assert-side-effect)
    assert(static_cast<NodeView::Type>(serial[index++]) == NodeView::Type::Hex);
    auto resource = deserialize_abstract_resource(serial[index++]);
    auto roll_number = static_cast<size_t>(serial[index++]);
    return Hex{ node, resource, roll_number };
}

} // namespace k10engine::Game::BoardView
