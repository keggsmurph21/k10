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
        os << "Port{ " << junction.port_exchange_rate() << ", " << junction.port_resources()
           << "} }";
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

std::vector<u8> Junction::serialize() const
{
    std::vector<u8> serial;
    serial.reserve(10);
    serial.push_back(static_cast<u8>(type()));
    for (const auto& byte : ::k10engine::serialize(port_resources()))
        serial.push_back(byte);
    serial.push_back(static_cast<u8>(port_exchange_rate()));
    serial.push_back(static_cast<u8>(has_settlement()));
    serial.push_back(static_cast<u8>(has_city()));
    serial.push_back(static_cast<u8>(is_settleable()));
    return serial;
}

Junction Junction::deserialize(const Board::Node& node, const std::vector<u8>& serial)
{
    size_t index = 0;
    // NOLINTNEXTLINE(bugprone-assert-side-effect)
    assert(static_cast<NodeView::Type>(serial[index++]) == NodeView::Type::Junction);
    auto port_resources = deserialize_resource_collection(serial, index);
    auto port_exchange_rate = static_cast<size_t>(serial[index++]);
    Junction junction{ node, port_resources, port_exchange_rate };

    if (static_cast<bool>(serial[index++]))
        junction.set_has_settlement();
    if (static_cast<bool>(serial[index++]))
        junction.set_has_city();
    if (!static_cast<bool>(serial[index++]))
        junction.set_is_not_settleable();

    return junction;
}

} // namespace k10engine::Game::BoardView
