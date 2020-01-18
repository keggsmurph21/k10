#include <assert.h>

#include "Board/Graph.h"

namespace k10engine {

namespace Board {

Graph::Graph(const NodeList nodes, const EdgeList edges)
    : m_num_hexes(0)
    , m_num_junctions(0)
    , m_num_oceans(0)
    , m_num_roads(0)
    , m_num_unflipped_hexes(0)
{
    for (auto const& item : nodes) {
        const int index = std::get<0>(item);
        const auto type = std::get<1>(item);
        assert(!has_node(index));
        auto node = new Node(index, type);
        m_nodes[index] = node;
        switch (type) {
        case NodeType::Hex:
            ++m_num_hexes;
            break;
        case NodeType::Ocean:
            ++m_num_oceans;
            break;
        case NodeType::Junction:
            ++m_num_junctions;
            break;
        case NodeType::Road:
            ++m_num_roads;
            break;
        case NodeType::UnflippedHex:
            ++m_num_unflipped_hexes;
            break;
        default:
            assert(false);
        }
    }
    for (auto const& item : edges) {
        auto node_0 = node(std::get<0>(item));
        auto node_1 = node(std::get<1>(item));
        const auto direction = std::get<2>(item);
        assert(node_0);
        assert(node_1);
        node_0->add_edge(direction, node_1);
    }
}

Graph::~Graph()
{
    for (auto item : m_nodes) {
        auto node = item.second;
        delete node;
    }
    m_nodes.clear();
}

Node* Graph::node(const int index)
{
    if (!has_node(index))
        return nullptr;
    return m_nodes.at(index);
}

bool Graph::has_node(const int index) const
{
    return m_nodes.find(index) != m_nodes.end();
}

} // namespace Board

} // namespace k10engine
