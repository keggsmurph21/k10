#pragma once

#include "Board/Graph.h"

namespace k10engine {

namespace Board {

class Base {
public:
    Graph* graph() { return m_graph; }

private:
    Graph* m_graph;
};

} // namespace Board

} // namespace k10engine
