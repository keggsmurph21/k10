#pragma once

#include <iostream>
#include <map>

#include "Board/Node.h"
#include "Core/Resource.h"

namespace k10engine::Game {

class Player;

namespace BoardView {

class Junction;

class Road {
public:
    const Player* owner() const { return m_owner; }
    void set_owner(Player* owner) { m_owner = owner; }

    size_t index() const { return m_node->index(); }
    const Board::Node* node() const { return m_node; }

    Road(const Board::Node* node)
        : m_node(node)
    {
    }

    friend std::ostream& operator<<(std::ostream&, const Road&);

private:
    const Board::Node* m_node;
    Player* m_owner{ nullptr };

    std::map<Board::Direction, const Junction*> m_junction_neighbors;

    void add_neighbor(const Board::Direction& direction, const Junction* junction)
    {
        m_junction_neighbors[direction] = junction;
    }
};

} // namespace BoardView

} // namespace k10engine::Game
