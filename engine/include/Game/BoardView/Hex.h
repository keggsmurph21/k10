#pragma once

#include <iostream>
#include <map>

#include "Board/Node.h"
#include "Core/Resource.h"
#include "Forward.h"

namespace k10engine::Game::BoardView {

template<typename T>
using Neighbors = std::map<Board::Direction, T*, std::less<>>;

class Hex {
public:
    const AbstractResource& resource() const { return m_resource; }
    size_t roll_number() const { return m_roll_number; }

    size_t index() const { return m_node.index(); }
    const Board::Node& node() const { return m_node; }

    Hex(const Board::Node& node, AbstractResource resource, size_t roll_number)
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

    Neighbors<Junction>& junction_neighbors() { return m_junction_neighbors; }
    const Neighbors<Junction>& junction_neighbors() const { return m_junction_neighbors; }
    void add_neighbor(Board::Direction direction, Junction* junction)
    {
        m_junction_neighbors[direction] = junction;
    }

private:
    const Board::Node& m_node;
    const AbstractResource m_resource;
    size_t m_roll_number; // 0 means <none>

    Neighbors<Junction> m_junction_neighbors;
};

} // namespace k10engine::Game::BoardView
