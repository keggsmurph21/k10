#pragma once

#include <vector>
#include <map>
#include <stdio.h>
#include <assert.h>

#define NUM_EDGES 6

namespace Graph {

enum class Direction {
    Clock12,
    Clock2,
    Clock4,
    Clock6,
    Clock8,
    Clock10,
};
Direction get_opposite(Direction);

class UndirectedGraph;
class Edge;

typedef std::map<Direction, Edge, std::less<>> EdgeMap;

class Node {
public:
    friend UndirectedGraph;

    ~Node() { m_edges.clear(); }

    constexpr int index() const { return m_index; }
    bool has_edge(Direction);
    Edge get_edge(Direction);
    void add_edge(Direction, Node*);

    void dump();

private:
    Node() {}
    Node(int index) : m_index(index) {}

    int m_index { -1 };
    EdgeMap m_edges;
};

class Edge {
public:
    friend Node;

    const Node* target() const { return m_target; }

private:
    Edge(Node* target) : m_target(target) {}
    Node* m_target;
};


class UndirectedGraph {
public:
    UndirectedGraph(int num_nodes) : m_size(num_nodes)
    {
        m_nodes = new Node[num_nodes];
        for (int i = 0; i < num_nodes; ++i)
            m_nodes[i] = Node(i);
    }

    ~UndirectedGraph()
    {
        delete[] m_nodes;
    }

    Node* operator[](int i) const
    {
        assert(0 <= i && i < size());
        return &m_nodes[i];
    }

    constexpr int size() const { return m_size; }

    void add_edge(int i, Direction dir, int j)
    {
        assert(0 <= i && i < size());
        assert(0 <= j && j < size());
        m_nodes[i].add_edge(dir, &m_nodes[j]);
        m_nodes[j].add_edge(get_opposite(dir), &m_nodes[i]);
    }

    void dump() {
        for (int i = 0; i < size(); ++i) {
            m_nodes[i].dump();
        }
    }

private:
    int m_size;
    Node* m_nodes;
};

}
