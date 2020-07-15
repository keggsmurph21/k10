#include <cassert>

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
        os << "Port{ " << junction.port_exchange_rate() << ", " << junction.port_resources() << "} }";
    }
    return os;
}

bool Junction::operator==(const NodeView& other) const
{
    return node() == other.node() && type() == other.type()
           && has_settlement() == static_cast<const Junction&>(other).has_settlement()
           && has_city() == static_cast<const Junction&>(other).has_city()
           && is_settleable() == static_cast<const Junction&>(other).is_settleable()
           && port_resources() == static_cast<const Junction&>(other).port_resources()
           && port_exchange_rate() == static_cast<const Junction&>(other).port_exchange_rate()
           && owner() == static_cast<const Junction&>(other).owner();
}

} // namespace k10engine::Game::BoardView

using Junction = k10engine::Game::BoardView::Junction;

template<>
void encode(ByteBuffer& buf, const Junction& junction)
{
    Encoder encoder(buf);
    encoder << junction.has_settlement();
    encoder << junction.has_city();
    encoder << junction.is_settleable();
    encoder << junction.port_exchange_rate();
    encoder << junction.port_resources();
    if (junction.owner() == nullptr) {
        encoder << false;
    } else {
        encoder << true;
        encoder << junction.owner()->index();
    }
}
