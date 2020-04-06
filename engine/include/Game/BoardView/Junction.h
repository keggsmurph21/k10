#pragma once

#include <iostream>

#include "Board/Direction.h"
#include "Core/Resource.h"
#include "Forward.h"
#include "Game/BoardView/Node.h"

namespace k10engine::Game::BoardView {

class Junction : public NodeView {
public:
    bool has_settlement() const { return m_has_settlement; }
    void set_has_settlement() { m_has_settlement = true; }

    bool has_city() const { return m_has_city; }
    void set_has_city() { m_has_city = true; }

    bool is_settleable() const { return m_is_settleable; }
    void set_is_not_settleable() { m_is_settleable = false; }

    bool is_port() const { return port_exchange_rate() > 0; }

    size_t port_exchange_rate() const { return m_port_exchange_rate; }
    const ResourceCollection& port_resources() const { return m_port_resources; }

    const Player* owner() const { return m_owner; }
    void set_owner(Player* owner) { m_owner = owner; }

    Junction(const Board::Node& node, ResourceCollection port_resources, size_t port_exchange_rate)
        : NodeView(node, NodeView::Type::Junction)
        , m_port_resources(port_resources)
        , m_port_exchange_rate(port_exchange_rate)
    {
    }

    friend std::ostream& operator<<(std::ostream&, const Junction&);

private:
    bool m_has_settlement{ false };
    bool m_has_city{ false };
    bool m_is_settleable{ true };
    const ResourceCollection m_port_resources;
    const size_t m_port_exchange_rate;
    Player* m_owner{ nullptr };
};

} // namespace k10engine::Game::BoardView
