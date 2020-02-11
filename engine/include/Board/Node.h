#pragma once

#include <iostream>
#include <map>
#include <tuple>
#include <vector>

#include "Board/Direction.h"
#include "Board/NodeType.h"

namespace k10engine {

namespace Board {

class Node {
public:
    Node(const size_t index, const NodeType type)
        : m_index(index)
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
    NodeType type() const { return m_type; }

private:
    const size_t m_index;
    const NodeType m_type;
};

} // namespace Board

} // namespace k10engine
