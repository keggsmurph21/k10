#include <cassert>

#include "Board/Port.h"

namespace k10engine::Board {

bool Port::contains(const Node* other_node) const
{
    if (other_node == nullptr) {
        return false;
    }
    for (const auto& my_node : m_nodes) {
        if (*my_node == *other_node) {
            return true;
        }
    }
    return false;
}

const Node* Port::buddy(const Node* other_node) const
{
    if (!contains(other_node)) {
        return nullptr;
    }
    for (const auto& my_node : m_nodes) {
        if (!(*my_node == *other_node)) {
            return my_node;
        }
    }
    assert(false);
}

std::vector<u8> Port::serialize() const
{
    return { static_cast<u8>(index()), static_cast<u8>(orientation()) };
}

Port Port::deserialize(std::set<const Node*> nodes, const std::vector<u8>& serial)
{
    size_t index = 0;
    auto port_index = static_cast<size_t>(serial[index++]);
    auto orientation = static_cast<Orientation>(serial[index++]);
    return { port_index, std::move(nodes), orientation };
}

} // namespace k10engine::Board
