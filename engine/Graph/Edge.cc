#include "Edge.h"
#include "Graph.h"
#include "Node.h"

#include <assert.h>

namespace Graph {

Orientation get_orientation(Direction dir)
{
    switch (dir) {
    case Direction::Clock12:
    case Direction::Clock6:
        return Orientation::Clock12Clock6;
    case Direction::Clock2:
    case Direction::Clock8:
        return Orientation::Clock2Clock8;
    case Direction::Clock4:
    case Direction::Clock10:
        return Orientation::Clock4Clock10;
    default:
        assert(false);
    }
}

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

} // namespace Graph
