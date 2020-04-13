#include <iostream>

#include "Board/Node.h"

namespace k10engine::Board {

std::ostream& operator<<(std::ostream& os, const Node& node)
{
    os << "Node{ " << node.index() << ", " << node.type() << " }";
    return os;
}

std::vector<u8> Node::serialize() const
{
    return { static_cast<u8>(index()),
             static_cast<u8>(x()),
             static_cast<u8>(y()),
             static_cast<u8>(type()) };
}

Node Node::deserialize(const std::vector<u8>& serial)
{
    size_t index = 0;
    auto node_index = static_cast<size_t>(serial[index++]);
    auto x = static_cast<size_t>(serial[index++]);
    auto y = static_cast<size_t>(serial[index++]);
    auto type = static_cast<NodeType>(serial[index++]);
    return { node_index, x, y, type };
}

} // namespace k10engine::Board
