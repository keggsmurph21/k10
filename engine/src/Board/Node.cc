#include <stdexcept>

#include "Board/Node.h"
#include "Board/Port.h"

namespace k10engine {

namespace Board {

Node::~Node()
{
    m_edges.clear();
}

std::ostream& operator<<(std::ostream& os, const Node& node)
{
    os << "Node(" << node.index() << ", " << node.type() << ")";
    return os;
}

bool Node::has_edge(const Direction d) const
{
    return m_edges.find(d) != m_edges.end();
}

const Node* Node::get_edge(const Direction d) const
{
    auto neighbor = m_edges.find(d);
    if (neighbor == m_edges.end()) {
        return nullptr;
    } else {
        return m_edges.at(d);
    }
}

void Node::add_edge(const Direction d, Node* n)
{
    if (has_edge(d))
        throw std::invalid_argument("Node already has neighbor in Direction");
    m_edges[d] = n;
}

void Node::set_port(const Port* port)
{
    if (!port)
        throw std::invalid_argument("Port cannot be nullptr");
    if (m_port)
        throw std::invalid_argument("Node already has Port");
    auto p = const_cast<Port*>(port);
    for (auto other_node : p->nodes()) {
        if (other_node == this)
            continue;
        for (auto direction : get_directions(p->orientation())) {
            if (auto first_neighbor = get_edge(direction))
                if (auto second_neighbor = first_neighbor->get_edge(direction))
                    if (*second_neighbor == *other_node)
                        return;
        }
    }
    throw std::invalid_argument("Node must be 'connected' to other Node in Port");
}

} // namespace Board

} // namespace k10engine
