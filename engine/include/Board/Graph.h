#pragma once

#include <map>
#include <utility>
#include <vector>

#include "Board/Direction.h"
#include "Board/Node.h"
#include "Board/NodeType.h"
#include "Board/Orientation.h"
#include "Board/Port.h"

namespace k10engine::Board {

typedef struct {
    size_t node_0_index;
    size_t node_1_index;
    Direction direction;
} EdgeSpec;

typedef struct {
    size_t node_0_index;
    size_t node_1_index;
    Orientation orientation;
} PortSpec;

typedef std::vector<NodeType> NodeSpecs;
typedef std::vector<EdgeSpec> EdgeSpecs;
typedef std::vector<PortSpec> PortSpecs;

typedef std::vector<const Node*> Nodes;
typedef std::map<std::pair<const Node*, const Direction>, const Node*, std::less<>> Edges;
typedef std::vector<const Port*> Ports;

class Graph {
public:
    Graph(const NodeSpecs&, const EdgeSpecs&, const PortSpecs&);
    ~Graph();

    const std::vector<const Node*>& nodes() const { return m_nodes; }
    const Node* node(const int) const;

    int size() const { return m_nodes.size(); }
    int size() { return m_nodes.size(); }

    bool has_neighbor(const Node*, const Direction) const;
    const Node* neighbor(const Node*, const Direction) const;
    std::vector<const Node*> neighbors(const Node* node) const;

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
