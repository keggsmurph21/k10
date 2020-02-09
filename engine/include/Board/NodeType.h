#pragma once

#include <iostream>

namespace k10engine::Board {

enum class NodeType {
    Hex,
    Ocean,
    Junction,
    Road,
    UnflippedHex,
};

std::ostream& operator<<(std::ostream&, NodeType);

} // namespace k10engine::Board
