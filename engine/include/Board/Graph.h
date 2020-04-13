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
    size_t width;
    size_t height;
} Dimensions;

typedef struct {
    size_t x;
    size_t y;
    NodeType type;
} NodeSpec;

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

typedef std::vector<NodeSpec> NodeSpecs;
typedef std::vector<EdgeSpec> EdgeSpecs;
typedef std::vector<PortSpec> PortSpecs;

typedef std::vector<Node> Nodes;
typedef std::map<std::pair<size_t, Direction>, size_t> Edges;
typedef std::vector<Port> Ports;

class Graph {
public:
    Graph(Dimensions, const NodeSpecs&, const EdgeSpecs&, const PortSpecs&);
    ~Graph();

    bool operator==(const Graph&) const;

    const Nodes& nodes() const { return m_nodes; }
    const Node* node(const size_t index) const;
    const Node* node(const size_t x, const size_t y) const;

    const Edges& edges() const { return m_edges; }
    const Ports& ports() const { return m_ports; }

    int size() const { return m_nodes.size(); }

    bool has_neighbor(const Node&, const Direction) const;
    const Node* neighbor(const Node&, const Direction) const;
    std::vector<const Node*> neighbors(const Node& node) const;

    const Port* port(const Node&) const;
    const Port* port(size_t port_index) const;

    size_t width() const { return m_dimensions.width; }
    size_t height() const { return m_dimensions.height; }

private:
    bool nodes_can_make_port(const Node&, const Node&, const Orientation);

    Nodes m_nodes;
    Edges m_edges;
    Ports m_ports;

    std::map<int, size_t> m_node_index_to_port_index_map;

    const Dimensions m_dimensions;
    std::vector<std::vector<int>> m_node_matrix;
};

} // namespace k10engine::Board
