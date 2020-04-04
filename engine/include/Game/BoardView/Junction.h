#pragma once

#include <iostream>
#include <map>

#include "Board/Direction.h"
#include "Board/Node.h"
#include "Core/Resource.h"

namespace k10engine::Game {

class Player;

namespace BoardView {

class Hex;
class Road;

template<typename T>
using Neighbors = std::map<Board::Direction, T*, std::less<>>;

class Junction {
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

    size_t index() const { return m_node.index(); }
    const Board::Node& node() const { return m_node; }

    Junction(const Board::Node& node, ResourceCollection port_resources, size_t port_exchange_rate)
        : m_node(node)
        , m_port_resources(port_resources)
        , m_port_exchange_rate(port_exchange_rate)
    {
    }

    friend std::ostream& operator<<(std::ostream&, const Junction&);

    Neighbors<Hex>& hex_neighbors() { return m_hex_neighbors; }
    const Neighbors<Hex>& hex_neighbors() const { return m_hex_neighbors; }
    void add_neighbor(Board::Direction direction, Hex* hex) { m_hex_neighbors[direction] = hex; }

    Neighbors<Road>& road_neighbors() { return m_road_neighbors; }
    const Neighbors<Road>& road_neighbors() const { return m_road_neighbors; }
    void add_neighbor(Board::Direction direction, Road* road)
    {
        m_road_neighbors[direction] = road;
    }

private:
    const Board::Node& m_node;
    bool m_has_settlement{ false };
    bool m_has_city{ false };
    bool m_is_settleable{ true };
    const ResourceCollection m_port_resources;
    const size_t m_port_exchange_rate;
    Player* m_owner{ nullptr };

    Neighbors<Hex> m_hex_neighbors;
    Neighbors<Road> m_road_neighbors;
};

} // namespace BoardView

} // namespace k10engine::Game
