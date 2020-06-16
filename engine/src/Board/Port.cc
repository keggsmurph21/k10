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

} // namespace k10engine::Board
