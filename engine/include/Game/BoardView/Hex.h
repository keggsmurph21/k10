#pragma once

#include <iostream>
#include <map>

#include "Core/Resource.h"
#include "Forward.h"
#include "Game/BoardView/Node.h"

namespace k10engine::Game::BoardView {

class Hex : public NodeView {
public:
    const AbstractResource& resource() const { return m_resource; }
    size_t roll_number() const { return m_roll_number; }

    Hex(const Board::Node& node, AbstractResource resource, size_t roll_number)
        : NodeView(node, NodeView::Type::Hex)
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
    const AbstractResource m_resource;
    size_t m_roll_number; // 0 means <none>

    Neighbors<Junction> m_junction_neighbors;
};

} // namespace k10engine::Game::BoardView
