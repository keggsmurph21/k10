#pragma once

#include <optional>
#include <vector>

#include "Board/Node.h"
#include "Core/Resource.h"

namespace k10engine {

namespace Game {

namespace BoardView {

class Hex {
public:
    AbstractResource resource() { return m_resource; }
    std::optional<int> roll_number() { return m_roll_number; }

    const Board::Node* node() const { return m_node; }

    Hex(Board::Node* node, AbstractResource resource, std::optional<int> roll_number)
        : m_node(node)
        , m_resource(resource)
        , m_roll_number(roll_number)
    {
    }

private:
    Board::Node* m_node;
    AbstractResource m_resource;
    std::optional<int> m_roll_number;
};

} // namespace BoardView

} // namespace Game

} // namespace k10engine
