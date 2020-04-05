#pragma once

#include "Board/Node.h"

namespace k10engine::Game::BoardView {

template<typename T>
using Neighbors = std::map<Board::Direction, T*, std::less<>>;

class NodeView {
public:
    enum class Type {
        Hex,
        Junction,
        Road,
    };

    const Board::Node& node() const { return m_node; }
    size_t index() const { return m_node.index(); }
    const Type& type() const { return m_type; }

private:
    const Board::Node& m_node;
    const Type m_type;

protected:
    NodeView(const Board::Node& node, Type type)
        : m_node(node)
        , m_type(type)
    {
    }
};

} // namespace k10engine::Game::BoardView
