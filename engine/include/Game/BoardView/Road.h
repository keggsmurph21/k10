#pragma once

#include <optional>

#include "Board/Road.h"
#include "Core/Resource.h"

namespace k10engine {

namespace Game {

namespace BoardView {

class Player;

class Road {
public:
    std::optional<Player*> owner() { return m_owner; }
    void set_owner(Player* owner) { m_owner = owner; }

    Road(Board::Road node)
        : m_node(node)
    {
    }

private:
    Board::Road m_node;
    std::optional<Player*> m_owner;
};

} // namespace BoardView

} // namespace Game

} // namespace k10engine

