#include <stdexcept>

#include "Board/Graph.h"

namespace k10engine::Board {

bool Graph::nodes_can_make_port(const Node* n0, const Node* n1, Orientation o)
{
    if (n0 == nullptr || n0->type() != NodeType::Junction) {
        return false;
    }
    if (n1 == nullptr || n1->type() != NodeType::Junction) {
        return false;
    }
    if (*n0 == *n1) {
        return false;
    }
    for (auto direction : get_directions(o)) {
        auto first_neighbor = neighbor(n0, direction);
        if (first_neighbor == nullptr) {
            continue;
        }
        auto second_neighbor = neighbor(first_neighbor, direction);
        if (second_neighbor == nullptr) {
            continue;
        }
        if (*n1 == *second_neighbor) {
            return true;
        }
    }
    return false;
}

Graph::Graph(const NodeSpecs& node_specs, const EdgeSpecs& edge_specs, const PortSpecs& port_specs)
{
    int index = 0;
    size_t port_index = 0;
    for (auto type : node_specs) {
        auto node = new Node(index, type);
        m_nodes.push_back(node);
        ++index;
    }
    for (const auto& it : edge_specs) {
        const auto node_0 = node(it.node_0_index);
        const auto node_1 = node(it.node_1_index);
        if (node_0 == nullptr || node_1 == nullptr) {
            goto clean_up_failed_construction;
        }
        if (has_neighbor(node_0, it.direction)) {
            goto clean_up_failed_construction;
        }
        std::pair<const Node*, const Direction> key = { node_0, it.direction };
        m_edges[key] = node_1;
    }
    for (auto it : port_specs) {
        const auto node_0 = node(it.node_0_index);
        const auto node_1 = node(it.node_1_index);
        if (!nodes_can_make_port(node_0, node_1, it.orientation)) {
            goto clean_up_failed_construction;
        }
        const auto port = new Port(port_index, { node_0, node_1 }, it.orientation);
        ++port_index;
        m_ports.push_back(port);
        // FIXME: Assert that we don't assign a port to an index if
        //        it's already in our map.  In that case, we should fail.
        m_node_index_to_port_map[node_0->index()] = port;
        m_node_index_to_port_map[node_1->index()] = port;
    }
    // FIXME: Make sure everything is connected (undirected)
    // for (auto g : (*m_graph)) {
    return;

clean_up_failed_construction:
    for (auto port : m_ports) {
        delete port;
    }
    m_ports.clear();
    m_edges.clear();
    for (auto node : m_nodes) {
        delete node;
    }
    m_nodes.clear();
    throw std::invalid_argument("Unable to construct Graph");
}

Graph::~Graph()
{
    for (auto port : m_ports) {
        delete port;
    }
    m_ports.clear();
    m_edges.clear();
    for (auto node : m_nodes) {
        delete node;
    }
    m_nodes.clear();
}

const Node* Graph::node(const int index) const
{
    // FIXME: Should these take in a size_t param?
    if (0 <= index && index < static_cast<int>(m_nodes.size())) {
        return m_nodes.at(index);
    }
    return nullptr;
}

bool Graph::has_neighbor(const Node* n, const Direction d) const
{
    return m_edges.find({ n, d }) != m_edges.end();
}

const Node* Graph::neighbor(const Node* n, const Direction d) const
{
    if (!has_neighbor(n, d)) {
        return nullptr;
    }
    return m_edges.at({ n, d });
}

const Port* Graph::port(const Node& node) const
{
    if (node.type() != NodeType::Junction) {
        return nullptr;
    }
    const auto node_index = node.index();
    if (m_node_index_to_port_map.find(node_index) == m_node_index_to_port_map.end()) {
        return nullptr;
    }
    return m_node_index_to_port_map.at(node_index);
}

const Port* Graph::port(size_t port_index) const
{
    if (port_index < m_ports.size()) {
        return m_ports.at(port_index);
    }
    return nullptr;
}

std::vector<const Node*> Graph::neighbors(const Node* node) const
{
    std::vector<const Node*> neighbors;
    for (const auto& direction : AllDirections) {
        const auto node_neighbor = neighbor(node, direction);
        if (node_neighbor != nullptr) {
            neighbors.push_back(node_neighbor);
        }
    }
    return neighbors;
}

} // namespace k10engine::Board
