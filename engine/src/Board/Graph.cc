#include <stdexcept>

#include "Board/Graph.h"

namespace k10engine {

namespace Board {

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

Graph::Graph(const _NodeSpecs node_specs, const _EdgeSpecs edge_specs, const _PortSpecs port_specs)
{
    int index = 0;
    for (auto type : node_specs) {
        auto node = new Node(index, type);
        m_nodes.push_back(node);
        ++index;
    }
    for (const auto& edge_spec : edge_specs) {
        const auto node_0 = node(std::get<0>(edge_spec));
        const auto node_1 = node(std::get<1>(edge_spec));
        const auto direction = std::get<2>(edge_spec);
        if (node_0 == nullptr || node_1 == nullptr) {
            goto clean_up_failed_construction;
        }
        if (has_neighbor(node_0, direction)) {
            goto clean_up_failed_construction;
        }
        std::pair<const Node*, const Direction> key = { node_0, direction };
        m_edges[key] = node_1;
    }
    for (auto port_spec : port_specs) {
        const auto node_0 = node(std::get<0>(port_spec));
        const auto node_1 = node(std::get<1>(port_spec));
        const auto orientation = std::get<2>(port_spec);
        if (!nodes_can_make_port(node_0, node_1, orientation)) {
            goto clean_up_failed_construction;
        }
        const auto port = new Port({ node_0, node_1 }, orientation);
        m_ports.push_back(port);
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

/*
Graph::Graph(const Graph& other)
{
    std::cout << "copy constructor" << std::endl;
    for (int i = 0; i < other.size(); ++i) {
        m_nodes.push_back(*other.node(i));
    }
}

Graph& Graph::operator=(const Graph& other)
{
    (void)other;
    std::cout << "copy assignment" << std::endl;
    throw std::invalid_argument("?!");
}
*/

const Node* Graph::node(const int index) const
{
    if (0 <= index && index < (int)m_nodes.size()) {
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

std::ostream& operator<<(std::ostream& os, NodeType type)
{
    switch (type) {
    case NodeType::Hex:
        os << "NodeType::Hex";
        break;
    case NodeType::Ocean:
        os << "NodeType::Ocean";
        break;
    case NodeType::Junction:
        os << "NodeType::Junction";
        break;
    case NodeType::Road:
        os << "NodeType::Road";
        break;
    case NodeType::UnflippedHex:
        os << "NodeType::UnflippedHex";
        break;
    default:
        os << "NodeType::Unknown";
    }
    return os;
}

} // namespace Board

} // namespace k10engine
