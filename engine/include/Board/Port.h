#pragma once

#include <map>
#include <set>
#include <stdexcept>

#include "Board/Node.h"
#include "Board/Orientation.h"

namespace k10engine {

namespace Board {

class Port {
public:
    Port(size_t index, std::set<const Node*> nodes, const Orientation o)
        : m_index(index)
        , m_nodes(nodes)
        , m_orientation(o)
    {
    }
    ~Port() {}

    size_t index() const { return m_index; }

    Orientation orientation() { return m_orientation; }
    const std::set<const Node*> nodes() { return m_nodes; }

private:
    const size_t m_index;
    std::set<const Node*> m_nodes;
    const Orientation m_orientation;
};

} // namespace Board

} // namespace k10engine
