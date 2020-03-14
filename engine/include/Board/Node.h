#pragma once

#include <iostream>
#include <map>
#include <vector>

#include "Board/Direction.h"
#include "Board/NodeType.h"

namespace k10engine::Board {

class Node {
public:
    Node(size_t index, size_t x, size_t y, NodeType type)
        : m_index(index)
        , m_x(x)
        , m_y(y)
        , m_type(type)
    {
    }
    ~Node() {}

    friend std::ostream& operator<<(std::ostream&, const Node&);

    bool operator<(Node other) { return index() < other.index(); }
    bool operator<(const Node other) const { return index() < other.index(); }

    bool operator==(Node other) { return index() == other.index() && type() == other.type(); }
    bool operator==(const Node other) const
    {
        return index() == other.index() && type() == other.type();
    }

    size_t index() const { return m_index; }
    size_t x() const { return m_x; }
    size_t y() const { return m_y; }
    NodeType type() const { return m_type; }

private:
    const size_t m_index;
    const size_t m_x;
    const size_t m_y;
    const NodeType m_type;
};

} // namespace k10engine::Board
