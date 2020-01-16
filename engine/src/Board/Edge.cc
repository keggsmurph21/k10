#include <assert.h>

#include "Board/Edge.h"
#include "Board/Node.h"

namespace k10engine {

namespace Board {

Edge::Edge(int index, Direction dir, Node* source, Node* target)
    : m_index(index)
    , m_source(source)
    , m_target(target)
{
    m_orientation = get_orientation(dir);
}

Edge::~Edge()
{
    std::cerr << "called ~Edge(" << index() << ")" << std::endl;
}

std::ostream& operator<<(std::ostream& cout, Edge& e)
{
    cout << "Edge(" << e.index() << ") {";
    for (int i = 0; i <= (int)Orientation::Clock4Clock10; ++i) {
        auto orientation = static_cast<Orientation>(i);
        if (e.orientation() == orientation) {
            cout << i << ":[";
            for (auto node : e.nodes()) {
                cout << "Node(" << node->index() << "),";
            }
            cout << "],";
        }
    }
    cout << "}";
    return cout;
}

int Edge::index() const
{
    return m_index;
}

Orientation Edge::orientation() const
{
    return m_orientation;
}

std::set<Node*> Edge::nodes() const
{
    assert(m_source && m_target);
    return { m_source, m_target };
}

} // namespace Board

} // namespace k10engine
