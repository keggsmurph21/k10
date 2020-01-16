#include <assert.h>

#include "Board/Node.h"

namespace k10engine {

namespace Board {

Node::Node() {}

Node::Node(int index)
    : m_index(index)
{
}

Node::~Node()
{
    std::cerr << "called ~Node(" << index() << ")" << std::endl;
    m_edges.clear();
}

std::ostream& operator<<(std::ostream& cout, Node& n)
{
    cout << "Node(" << n.index() << ") {";
    for (int i = 0; i <= (int)Direction::Clock10; ++i) {
        auto dir = static_cast<Direction>(i);
        if (n.has_edge(dir)) {
            // cout << i << ":Edge(" << n.get_edge(dir)->index() << "),";
        }
    }
    cout << "}";
    return cout;
}

int Node::index() const
{
    return m_index;
}

bool Node::has_edge(Direction dir)
{
    return m_edges.find(dir) != m_edges.end();
}

Node* Node::get_edge(Direction dir)
{
    auto edge = m_edges.find(dir);
    assert((edge != m_edges.end()));
    return edge->second;
}

void Node::add_edge(Direction dir, Node* node)
{
    assert(!has_edge(dir));
    m_edges.insert_or_assign(dir, node);
}

} // namespace Board

} // namespace k10engine
