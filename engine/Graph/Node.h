#pragma once

#include <iostream>
#include <map>

namespace Graph {

enum class Direction;
class Edge;

class Node {
public:
    Node();
    Node(int);
    ~Node();

    int index() const;
    bool has_edge(Direction);
    Node* get_edge(Direction);

    void add_edge(Direction, Node*);

private:
    friend std::ostream& operator<<(std::ostream&, Node&);

    typedef std::map<Direction, Node*, std::less<>> EdgeMap;

    int m_index { -1 };
    EdgeMap m_edges;
};

}