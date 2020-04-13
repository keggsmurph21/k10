#pragma once

#include <map>
#include <set>
#include <stdexcept>

#include "Board/Node.h"
#include "Board/Orientation.h"
#include "Util/Serializable.h"

namespace k10engine::Board {

class Port : public Serializable {
public:
    Port(size_t index, std::set<const Node*> nodes, const Orientation o)
        : m_index(index)
        , m_nodes(std::move(nodes))
        , m_orientation(o)
    {
    }
    ~Port() {}

    bool operator==(const Port& other) const
    {
        return index() == other.index() && orientation() == other.orientation();
    }

    size_t index() const { return m_index; }

    Orientation orientation() const { return m_orientation; }
    const std::set<const Node*>& nodes() const { return m_nodes; }

    bool contains(const Node*) const;
    const Node* buddy(const Node*) const;

    std::vector<u8> serialize() const override;
    static Port deserialize(std::set<const Node*>, const std::vector<u8>&);

private:
    const size_t m_index;
    std::set<const Node*> m_nodes;
    const Orientation m_orientation;
};

} // namespace k10engine::Board
