#include "Graph.h"

namespace Graph {

Direction get_opposite(Direction dir)
{
    if (dir == Direction::Clock12)
        return Direction::Clock6;
    if (dir == Direction::Clock2)
        return Direction::Clock8;
    if (dir == Direction::Clock4)
        return Direction::Clock10;
    if (dir == Direction::Clock6)
        return Direction::Clock12;
    if (dir == Direction::Clock8)
        return Direction::Clock2;
    if (dir == Direction::Clock10)
        return Direction::Clock4;
    assert(false);
}

bool operator<(const Direction d1, const Direction d2)
{
    return (int)d1 < (int)d2;
}

bool Node::has_edge(Direction dir)
{
    return m_edges.find(dir) != m_edges.end();
}

Edge Node::get_edge(Direction dir)
{
    auto edge = m_edges.find(dir);
    assert((edge != m_edges.end()));
    return edge->second;
}

void Node::add_edge(Direction dir, Node* target)
{
    assert(!has_edge(dir));
    m_edges.insert_or_assign(dir, Edge(target));
}

void Node::dump()
{
    printf("Node(%d){", index());
    for (int i = 0; i <= (int)Direction::Clock10; ++i) {
        Direction dir = static_cast<Direction>(i);
        if (has_edge(dir))
            printf("%d", get_edge(dir).target()->index());
        printf(",");
    }
    printf("}\n");
}

}
