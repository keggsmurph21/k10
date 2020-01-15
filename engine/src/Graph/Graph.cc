#include <assert.h>
#include <iostream>

#include "Graph.h"

namespace Graph {

Direction get_opposite(Direction dir)
{
    switch (dir) {
    case Direction::Clock12:
        return Direction::Clock6;
    case Direction::Clock2:
        return Direction::Clock8;
    case Direction::Clock4:
        return Direction::Clock10;
    case Direction::Clock6:
        return Direction::Clock12;
    case Direction::Clock8:
        return Direction::Clock2;
    case Direction::Clock10:
        return Direction::Clock4;
    default:
        assert(false);
    }
}

bool operator<(const Direction d1, const Direction d2)
{
    return (int)d1 < (int)d2;
}

Graph::Graph(int n_roads, int n_junctions, int n_hexes, int n_flippables, int n_oceans)
{
    for (int i = 0; i < n_roads; ++i)
        m_roads.push_back(new Road(i));
    for (int i = 0; i < n_junctions; ++i)
        m_junctions.push_back(new Junction(i));
    for (int i = 0; i < n_hexes; ++i)
        m_hexes.push_back(new Hex(i));
    for (int i = 0; i < n_flippables; ++i)
        m_flippables.push_back(new Flippable(i));
    for (int i = 0; i < n_oceans; ++i)
        m_oceans.push_back(new Ocean(i));
}

Graph::~Graph()
{
    std::cerr << "called ~Graph" << std::endl;
    for (auto it : m_roads)
        delete it;
    for (auto it : m_junctions)
        delete it;
    for (auto it : m_hexes)
        delete it;
    for (auto it : m_flippables)
        delete it;
    for (auto it : m_oceans)
        delete it;
}

std::ostream& operator<<(std::ostream& cout, Graph& g)
{
    cout << "Graph(" << /*g.size() <<*/ ")" << std::endl;
    (void)g;
    /*
    cout << "  nodes:" << std::endl;
    for (auto& node : g.m_nodes)
        cout << "    " << *node << std::endl;
    cout << "  edges:" << std::endl;
    for (auto& edge : g.m_edges)
        cout << "    " << *edge << std::endl;
        */
    return cout;
}

Road* Graph::road(int index)
{
    return m_roads[index];
}

Junction* Graph::junction(int index)
{
    return m_junctions[index];
}

Hex* Graph::hex(int index)
{
    return m_hexes[index];
}

Flippable* Graph::flippable(int index)
{
    return m_flippables[index];
}

Ocean* Graph::ocean(int index)
{
    return m_oceans[index];
}

}
