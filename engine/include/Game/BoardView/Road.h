#pragma once

#include <iostream>
#include <map>

#include "Core/Resource.h"
#include "Forward.h"
#include "Game/BoardView/Node.h"

namespace k10engine::Game::BoardView {

class Road : public NodeView {
public:
    const Player* owner() const { return m_owner; }
    void set_owner(Player* owner) { m_owner = owner; }

    Road(const Board::Node& node)
        : NodeView(node, NodeView::Type::Road)
    {
    }

    friend std::ostream& operator<<(std::ostream&, const Road&);

    Neighbors<Junction>& junction_neighbors() { return m_junction_neighbors; }
    const Neighbors<Junction>& junction_neighbors() const { return m_junction_neighbors; }
    void add_neighbor(Board::Direction direction, Junction* junction)
    {
        m_junction_neighbors[direction] = junction;
    }

private:
    Player* m_owner{ nullptr };

    Neighbors<Junction> m_junction_neighbors;
};

} // namespace k10engine::Game::BoardView
