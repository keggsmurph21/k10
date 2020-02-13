#include <iostream>
#include <stdexcept>

#include "Board/Node.h"

namespace k10engine::Board {

std::ostream& operator<<(std::ostream& os, const Node& node)
{
    os << "Node{ " << node.index() << ", " << node.type() << " }";
    return os;
}

} // namespace k10engine::Board
