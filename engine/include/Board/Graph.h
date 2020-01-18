#pragma once

#include <tuple>
#include <vector>

#include "Board/Direction.h"
#include "Board/Node.h"
#include "Board/NodeType.h"

namespace k10engine {

namespace Board {

typedef std::vector<std::tuple<int, NodeType>> NodeList;
typedef std::vector<std::tuple<int, int, Direction>> EdgeList;

class Graph {
public:
    Graph(const NodeList, const EdgeList);
    ~Graph();

    Node* node(const int);
    const std::map<int, Node*, std::less<>> nodes() { return m_nodes; }

    int num_hexes() const { return m_num_hexes; }
    int num_junctions() const { return m_num_junctions; }
    int num_oceans() const { return m_num_oceans; }
    int num_roads() const { return m_num_roads; }
    int num_unflipped_hexes() const { return m_num_unflipped_hexes; }

private:
    bool has_node(const int) const;

    std::map<int, Node*, std::less<>> m_nodes;
    int m_num_hexes;
    int m_num_junctions;
    int m_num_oceans;
    int m_num_roads;
    int m_num_unflipped_hexes;
};

} // namespace Board

} // namespace k10engine
