#pragma once

#include "Node.h"

namespace k10engine {

namespace Graph {

class Node;

class Junction : public Node {
public:
    Junction(int index)
        : m_index(index)
    {
        (void)m_index;
    }

private:
    int m_index;
};

} // namespace Graph

} // namespace k10engine
