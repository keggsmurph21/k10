#pragma once

#include <optional>
#include <vector>

#include "Board/Hex.h"
#include "Game/Resource.h"

namespace k10engine {

namespace Game {

namespace BoardView {

class Hex {
public:
    Resource resource() { return m_resource; }
    int roll_number() { return m_roll_number; }

    Hex(Board::Hex node, Resource resource, int roll_number)
        : m_node(node)
        , m_resource(resource)
        , m_roll_number(roll_number)
    {
    }

private:
    Board::Hex m_node;
    Resource m_resource;
    int m_roll_number;
};

} // namespace BoardView

} // namespace Game

} // namespace k10engine
