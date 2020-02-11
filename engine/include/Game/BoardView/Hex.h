#pragma once

#include <iostream>
#include <map>

#include "Board/Node.h"
#include "Core/Resource.h"

namespace k10engine::Game::BoardView {

class Junction;
class Road;

class Hex {
public:
    const AbstractResource& resource() const { return m_resource; }
    size_t roll_number() const { return m_roll_number; }

    size_t index() const { return m_node->index(); }
    const Board::Node* node() const { return m_node; }

    Hex(const Board::Node* node, const AbstractResource resource, size_t roll_number)
        : m_node(node)
        , m_resource(resource)
        , m_roll_number(roll_number)
    {
    }

    friend std::ostream& operator<<(std::ostream& os, const Hex& hex)
    {
        os << "BoardView::Hex{ " << hex.index() << ", " << hex.resource() << ", "
           << hex.roll_number() << " }";
        return os;
    }

private:
    const Board::Node* m_node;
    const AbstractResource m_resource;
    size_t m_roll_number; // 0 means <none>

    std::map<Board::Direction, const Junction*, std::less<>> m_junction_neighbors;
    std::map<Board::Direction, const Road*, std::less<>> m_road_neighbors;

    void add_neighbor(const Board::Direction& direction, const Junction* junction)
    {
        m_junction_neighbors[direction] = junction;
    }
    void add_neighbor(const Board::Direction& direction, const Road* road)
    {
        m_road_neighbors[direction] = road;
    }
};

} // namespace k10engine::Game::BoardView
