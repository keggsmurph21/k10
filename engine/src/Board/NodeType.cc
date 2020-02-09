#include "Board/NodeType.h"

namespace k10engine::Board {

std::ostream& operator<<(std::ostream& os, NodeType type)
{
    switch (type) {
    case NodeType::Hex:
        os << "NodeType::Hex";
        break;
    case NodeType::Ocean:
        os << "NodeType::Ocean";
        break;
    case NodeType::Junction:
        os << "NodeType::Junction";
        break;
    case NodeType::Road:
        os << "NodeType::Road";
        break;
    case NodeType::UnflippedHex:
        os << "NodeType::UnflippedHex";
        break;
    default:
        os << "NodeType::Unknown";
    }
    return os;
}

} // namespace k10engine::Board
