#pragma once

#include <iostream>
#include <map>
#include <tuple>
#include <vector>

#include "Board/Direction.h"
#include "Board/NodeType.h"

namespace k10engine {

namespace Board {

class Port;

class Node {
public:
    Node(const int index, const NodeType type)
        : m_index(index)
        , m_type(type)
        , m_port(nullptr)
    {
    }
    ~Node();

    friend std::ostream& operator<<(std::ostream&, const Node&);

    bool operator==(Node other) { return index() == other.index() && type() == other.type(); }
    bool operator==(const Node other) const
    {
        return index() == other.index() && type() == other.type();
    }

    int index() const { return m_index; }
    NodeType type() const { return m_type; }

    typedef std::map<const Direction, Node*, std::less<>> EdgeMap;

    bool has_edge(const Direction) const;
    const Node* get_edge(const Direction) const;
    void add_edge(const Direction, Node*);

    void set_port(const Port*);
    const Port* port() { return m_port; }

    const EdgeMap* neighbors() const { return &m_edges; }

private:
    const int m_index;
    const NodeType m_type;
    EdgeMap m_edges;
    Port* m_port;
};

} // namespace Board

} // namespace k10engine
