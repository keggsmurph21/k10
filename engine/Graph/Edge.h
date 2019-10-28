#pragma once

#include <iostream>
#include <set>

#include "Graph.h"
#include "Node.h"

namespace Graph {


enum class Direction;
enum class Orientation {
    Clock12Clock6,
    Clock2Clock8,
    Clock4Clock10,
};
Orientation get_orientation(Direction);

class Graph;
class Node;

class Edge {
public:
    friend Graph;
    ~Edge();

    friend std::ostream& operator<<(std::ostream&, Edge&);

    int index() const;
    Orientation orientation() const;
    std::set<Node*> nodes() const;

private:
    Edge(int, Direction, Node*, Node*);

    int m_index;
    Orientation m_orientation;
    Node* m_source;
    Node* m_target;
};

}