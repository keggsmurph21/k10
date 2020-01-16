#pragma once

#include <optional>

#include "Board/Junction.h"
#include "Core/Resource.h"

namespace k10engine {

namespace Game {

namespace BoardView {

class Player;

class Junction {
    bool has_settlement() { return m_has_settlement; }
    void set_has_settlement() { m_has_settlement = true; }

    bool has_city() { return m_has_city; }
    void set_has_city() { m_has_city = true; }

    bool is_settleable() { return m_is_settleable; }
    void set_is_not_settleable() { m_is_settleable = false; }

    std::optional<Resource> port_type() { return m_port_type; }

    std::optional<Player*> owner() { return m_owner; }
    void set_owner(Player* owner) { m_owner = owner; }

    Junction(Board::Junction node)
        : m_node(node)
    {
        m_has_settlement = false;
        m_has_city = false;
        m_is_settleable = true;
        // FIXME:
        // m_port_type = ???;
        m_owner = std::nullopt;
    }

private:
    Board::Junction m_node;
    bool m_has_settlement;
    bool m_has_city;
    bool m_is_settleable;
    std::optional<Resource> m_port_type;
    std::optional<Player*> m_owner;
};

} // namespace BoardView

} // namespace Game

} // namespace k10engine
