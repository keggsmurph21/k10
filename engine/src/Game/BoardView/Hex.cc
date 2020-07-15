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

Hex* Hex::decode(ByteBuffer& buf, const Board::Node& node)
{
    Decoder decoder(buf);
    AbstractResource resource;
    if (!decoder.decode(resource))
        return nullptr;
    size_t roll_number;
    if (!decoder.decode(roll_number))
        return nullptr;
    return new Hex(node, resource, roll_number);
}

} // namespace k10engine::Game::BoardView

using Hex = k10engine::Game::BoardView::Hex;

template<>
void encode(ByteBuffer& buf, const Hex& hex)
{
    Encoder encoder(buf);
    encoder << hex.resource();
    encoder << hex.roll_number();
}
