#pragma once

#include <iostream>
#include <set>

#include "Board/Direction.h"
#include "Board/Graph.h"
#include "Board/Node.h"
#include "Board/Orientation.h"

namespace k10engine {

namespace Board {

class Graph;

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

} // namespace Board

} // namespace k10engine
