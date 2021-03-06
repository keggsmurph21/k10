#pragma once

#include <map>
#include <set>
#include <stdexcept>

#include "Board/Node.h"
#include "Board/Orientation.h"

namespace k10engine::Board {

class Port {
public:
    Port(size_t index, std::set<const Node*> nodes, const Orientation o)
        : m_index(index)
        , m_nodes(std::move(nodes))
        , m_orientation(o)
    {
    }
    ~Port() {}

    bool operator==(const Port& other) const
    {
        return index() == other.index() && orientation() == other.orientation();
    }

    size_t index() const { return m_index; }

    Orientation orientation() const { return m_orientation; }
    const std::set<const Node*>& nodes() const { return m_nodes; }

    bool contains(const Node*) const;
    const Node* buddy(const Node*) const;

private:
    const size_t m_index;
    std::set<const Node*> m_nodes;
    const Orientation m_orientation;
};

} // namespace k10engine::Board
