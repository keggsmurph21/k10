#include <assert.h>

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
    assert(!has_edge(d));
    m_edges[d] = n;
}

void Node::set_port(const Port* port)
{
    assert(port);
    assert(!m_port);
    bool connected_to_other_node = false;
    auto p = const_cast<Port*>(port);
    for (auto other_node : p->nodes()) {
        if (other_node == this)
            continue;
        for (auto direction : get_directions(p->orientation())) {
            if (auto first_neighbor = get_edge(direction))
                if (auto second_neighbor = first_neighbor->get_edge(direction))
                    connected_to_other_node |= (*second_neighbor == *other_node);
        }
    }
    assert(connected_to_other_node);
}

} // namespace Board

} // namespace k10engine
