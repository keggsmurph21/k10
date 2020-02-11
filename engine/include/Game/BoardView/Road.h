#pragma once

#include "Board/Node.h"
#include "Core/Resource.h"

namespace k10engine {

namespace Game {

namespace BoardView {

class Player;

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

private:
    const Board::Node* m_node;
    Player* m_owner{ nullptr };
};

} // namespace BoardView

} // namespace Game

} // namespace k10engine

