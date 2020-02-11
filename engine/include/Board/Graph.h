#pragma once

#include <map>
#include <tuple>
#include <utility>
#include <vector>

#include "Board/Direction.h"
#include "Board/Node.h"
#include "Board/NodeType.h"
#include "Board/Orientation.h"
#include "Board/Port.h"

namespace k10engine::Board {

typedef std::vector<NodeType> _NodeSpecs;
typedef std::vector<std::tuple<int, int, Direction>> _EdgeSpecs;
typedef std::vector<std::tuple<int, int, Orientation>> _PortSpecs;

typedef std::vector<const Node*> Nodes;
typedef std::map<std::pair<const Node*, const Direction>, const Node*, std::less<>> Edges;
typedef std::vector<const Port*> Ports;

class Base;

class Graph {
public:
    Graph(const _NodeSpecs&, const _EdgeSpecs&, const _PortSpecs&);
    ~Graph();
    // Graph(const Graph&);
    // Graph& operator=(const Graph&);

    const std::vector<const Node*>& nodes() const { return m_nodes; }
    const Node* node(const int) const;

    int size() const { return m_nodes.size(); }
    int size() { return m_nodes.size(); }

    using ConstIterator = std::vector<const Node*>::const_iterator;
    bool has_neighbor(const Node*, const Direction) const;
    const Node* neighbor(const Node*, const Direction) const;
    ConstIterator neighbors(const Node*) const;

    const Port* port(const Node&) const;
    const Port* port(size_t port_index) const;

private:
    bool nodes_can_make_port(const Node*, const Node*, const Orientation);

    Nodes m_nodes;
    Edges m_edges;
    Ports m_ports;

    std::map<int, const Port*> m_node_index_to_port_map;
};

} // namespace k10engine::Board
