#include <assert.h>

#include "Board/Base.h"

namespace k10engine {

namespace Board {

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

Base::Base(const std::string name,
           const Graph* graph,
           const std::vector<std::tuple<int, int, Orientation>> port_specs)
    : m_name(name)
    , m_graph(graph)
{
    auto g = const_cast<Graph*>(m_graph);
    for (auto port_spec : port_specs) {
        auto node_0 = g->node(std::get<0>(port_spec));
        auto node_1 = g->node(std::get<1>(port_spec));
        const auto orientation = std::get<2>(port_spec);
        const auto port = new Port({ node_0, node_1 }, orientation);
        m_ports.push_back(port);
        node_0->set_port(port);
        node_1->set_port(port);
    }
    // FIXME: Make sure everything is connected (undirected)
    // for (auto g : (*m_graph)) {
}

Base::~Base()
{
    delete m_graph;
    for (auto port : m_ports) {
        delete port;
    }
    m_ports.clear();
}

// bool Base::has(std::string name)
// {
// }

// Base* Base::get(std::string name)
// {
// }

} // namespace Board

} // namespace k10engine
