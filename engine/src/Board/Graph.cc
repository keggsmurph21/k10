#include <algorithm>
#include <stdexcept>

#include "Board/Graph.h"

namespace k10engine::Board {

bool Graph::nodes_can_make_port(const Node& n0, const Node& n1, Orientation o) const
{
    if (n1.type() != NodeType::Junction) {
        return false;
    }
    if (n1.type() != NodeType::Junction) {
        return false;
    }
    if (n0 == n1) {
        return false;
    }
    for (auto direction : get_directions(o)) {
        auto first_neighbor = neighbor(n0, direction);
        if (first_neighbor == nullptr) {
            continue;
        }
        auto second_neighbor = neighbor(*first_neighbor, direction);
        if (second_neighbor == nullptr) {
            continue;
        }
        if (n1 == *second_neighbor) {
            return true;
        }
    }
    return false;
}

Graph::Graph(Dimensions dimensions,
             const NodeSpecs& node_specs,
             const EdgeSpecs& edge_specs,
             const PortSpecs& port_specs)
    : m_dimensions(dimensions)
{
    m_node_matrix.reserve(dimensions.height);
    for (size_t i = 0; i < dimensions.height; ++i) {
        std::vector<int> row(dimensions.width);
        std::fill(row.begin(), row.end(), -1);
        m_node_matrix.push_back(row);
    }
    int index = 0;
    size_t port_index = 0;
    for (const auto& it : node_specs) {
        m_nodes.push_back(Node(index, it.x, it.y, it.type));
        m_node_matrix[it.y][it.x] = index;
        ++index;
    }
    for (const auto& it : edge_specs) {
        const auto node_0 = node(it.node_0_index);
        const auto node_1 = node(it.node_1_index);
        if (node_0 == nullptr || node_1 == nullptr) {
            goto clean_up_failed_construction;
        }
        if (has_neighbor(*node_0, it.direction)) {
            goto clean_up_failed_construction;
        }
        std::pair<size_t, Direction> key = { node_0->index(), it.direction };
        m_edges[key] = node_1->index();
    }
    for (const auto& it : port_specs) {
        const auto node_0 = node(it.node_0_index);
        const auto node_1 = node(it.node_1_index);
        if (node_0 == nullptr || node_1 == nullptr || !nodes_can_make_port(*node_0, *node_1, it.orientation)) {
            goto clean_up_failed_construction;
        }
        m_ports.push_back(Port(port_index, { node_0, node_1 }, it.orientation));
        // FIXME: Assert that we don't assign a port to an index if
        //        it's already in our map.  In that case, we should fail.
        m_node_index_to_port_index_map[node_0->index()] = port_index;
        m_node_index_to_port_index_map[node_1->index()] = port_index;
        ++port_index;
    }
    // FIXME: Make sure everything is connected (undirected)
    // for (auto g : (*m_graph)) {
    return;

clean_up_failed_construction:
    m_ports.clear();
    m_edges.clear();
    m_nodes.clear();
    throw std::invalid_argument("Unable to construct Graph");
}

Graph::~Graph()
{
    m_ports.clear();
    m_edges.clear();
    m_nodes.clear();
}

bool Graph::operator==(const Graph& other) const
{
    if (nodes().size() != other.nodes().size())
        return false;
    for (size_t i = 0; i < nodes().size(); ++i) {
        if (!(nodes().at(i) == other.nodes().at(i)))
            return false;
    }

    if (edges().size() != other.edges().size())
        return false;
    for (const auto& my_it : edges()) {
        const auto& my_key = my_it.first;
        const auto& other_value = other.edges().find(my_key);
        if (other_value == other.edges().end())
            return false;
        if (!(my_it.second == other_value->second))
            return false;
    }

    if (ports().size() != other.ports().size())
        return false;
    for (size_t i = 0; i < ports().size(); ++i) {
        if (!(ports().at(i) == other.ports().at(i)))
            return false;
    }

    return true;
}

const Node* Graph::node(const size_t index) const
{
    if (index < m_nodes.size()) {
        return &m_nodes.at(index);
    }
    return nullptr;
}

const Node* Graph::node(const size_t x, const size_t y) const
{
    if (x >= width()) {
        return nullptr;
    }
    if (y >= height()) {
        return nullptr;
    }
    const auto index = m_node_matrix.at(y).at(x);
    if (index < 0) {
        return nullptr;
    }
    return node(index);
}

bool Graph::has_neighbor(const Node& n, const Direction d) const
{
    return m_edges.find({ n.index(), d }) != m_edges.end();
}

const Node* Graph::neighbor(const Node& n, const Direction d) const
{
    if (!has_neighbor(n, d)) {
        return nullptr;
    }
    const auto index = m_edges.at({ n.index(), d });
    return node(index);
}

const Port* Graph::port(const Node& node) const
{
    if (node.type() != NodeType::Junction) {
        return nullptr;
    }
    const auto node_index = node.index();
    if (m_node_index_to_port_index_map.find(node_index) == m_node_index_to_port_index_map.end()) {
        return nullptr;
    }
    const auto port_index = m_node_index_to_port_index_map.at(node_index);
    return port(port_index);
}

const Port* Graph::port(size_t port_index) const
{
    if (port_index < m_ports.size()) {
        return &m_ports.at(port_index);
    }
    return nullptr;
}

std::vector<const Node*> Graph::neighbors(const Node& node) const
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
