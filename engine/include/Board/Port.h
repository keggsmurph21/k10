#pragma once

#include <assert.h>
#include <map>
#include <set>

#include "Board/Node.h"
#include "Board/Orientation.h"

namespace k10engine {

namespace Board {

class Port {
public:
    Port(std::set<const Node*> nodes, const Orientation o)
        : m_nodes(nodes)
        , m_orientation(o)
    {
        for (auto node : m_nodes) {
            assert(node);
            assert(node->type() == NodeType::Junction);
        }
    }
    ~Port() { m_nodes.clear(); }

    Orientation orientation() { return m_orientation; }
    const std::set<const Node*> nodes() { return m_nodes; }

private:
    std::set<const Node*> m_nodes;
    const Orientation m_orientation;
};

} // namespace Board

} // namespace k10engine
