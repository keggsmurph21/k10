#pragma once

#include <iostream>
#include <map>

#include "Board/Node.h"
#include "Core/Resource.h"

namespace k10engine::Game {

class Player;

namespace BoardView {

class Junction;

template<typename T>
using Neighbors = std::map<Board::Direction, const T*, std::less<>>;

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

    const Neighbors<Junction>& junction_neighbors() const { return m_junction_neighbors; }
    void add_neighbor(Board::Direction direction, const Junction* junction)
    {
        m_junction_neighbors[direction] = junction;
    }

private:
    const Board::Node* m_node;
    Player* m_owner{ nullptr };

    Neighbors<Junction> m_junction_neighbors;
};

} // namespace BoardView

} // namespace k10engine::Game
