#pragma once

#include <vector>

#include "Board/Node.h"
#include "Core/Resource.h"

namespace k10engine::Game::BoardView {

class Hex {
public:
    const AbstractResource& resource() const { return m_resource; }
    std::optional<int> roll_number() { return m_roll_number; }

    const Board::Node* node() const { return m_node; }

    Hex(const Board::Node* node, const AbstractResource& resource, int roll_number)
        : m_node(node)
        , m_resource(resource)
        , m_roll_number(roll_number)
    {
    }

private:
    const Board::Node* m_node;
    const AbstractResource& m_resource;
    int m_roll_number; // 0 means <none>
};

} // namespace k10engine::Game::BoardView
