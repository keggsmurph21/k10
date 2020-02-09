#pragma once

#include "Board/Node.h"
#include "Core/Resource.h"

namespace k10engine {

namespace Game {

namespace BoardView {

class Player;

class Junction {
public:
    bool has_settlement() { return m_has_settlement; }
    void set_has_settlement() { m_has_settlement = true; }

    bool has_city() { return m_has_city; }
    void set_has_city() { m_has_city = true; }

    bool is_settleable() { return m_is_settleable; }
    void set_is_not_settleable() { m_is_settleable = false; }

    const ResourceCollection& port_resources() { return m_port_resources; }

    const Player* owner() const { return m_owner; }
    void set_owner(Player* owner) { m_owner = owner; }

    const Board::Node* node() const { return m_node; }

    Junction(const Board::Node* node,
             const ResourceCollection& port_resources,
             size_t port_exchange_rate)
        : m_node(node)
        , m_port_resources(port_resources)
        , m_port_exchange_rate(port_exchange_rate)
    {
    }

private:
    const Board::Node* m_node;
    bool m_has_settlement{ false };
    bool m_has_city{ false };
    bool m_is_settleable{ true };
    const ResourceCollection& m_port_resources;
    const size_t m_port_exchange_rate;
    Player* m_owner{ nullptr };
};

} // namespace BoardView

} // namespace Game

} // namespace k10engine
