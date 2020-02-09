#pragma once

#include <optional>

#include "Board/Node.h"
#include "Core/Resource.h"

namespace k10engine {

namespace Game {

namespace BoardView {

class Player;

class Road {
public:
    std::optional<Player*> owner() { return m_owner; }
    void set_owner(Player* owner) { m_owner = owner; }

    const Board::Node* node() const { return m_node; }

    Road(const Board::Node* node)
        : m_node(node)
    {
    }

private:
    const Board::Node* m_node;
    std::optional<Player*> m_owner;
};

} // namespace BoardView

} // namespace Game

} // namespace k10engine

